#include <PCH.h>
#include <Rendering/RenderContext.h>
#include <Rendering/DescriptorPool.h>
#include <Rendering/DynamicUploadHeap.h>
#include <Rendering/CommandListPool.h>
#include <Rendering/ShaderFactory.h>
#include <Rendering/UIRenderContext.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineCoreMediator.h>
#include <Core/TaskManager.h>
#include <RHI/DebugLayer.h>
#include <RHI/Device.h>
#include <RHI/Fence.h>
#include <RHI/FrameFence.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandList.h>
#include <RHI/PIXMarker.h>
#include <RHI/Shader.h>
#include <RHI/Texture.h>

namespace sy
{
	RenderContext::RenderContext(HWND windowHandle, const CommandLineParser& commandLineParser, const ShaderFactory& shaderFactory) :
		adapterPatcher(commandLineParser),
		renderResolution({ 1280, 720 }),
	    currentFrame(0),
	    shaderFactory(shaderFactory)
	{
		Logger& logger = EngineCore::GetLogger();
	    TaskManager& taskManager = EngineCore::GetTaskManager();

		if (commandLineParser.ShouldEnableDebugLayer())
		{
			logger.info("Trying to enable DX12 debug layer...");
			RHI::DebugLayer::Enable();
			logger.info("DX12 debug layer enabled.");
		}

		logger.info("Initializing RenderContext...");
		{
			logger.info("Creating Device...");
			device = std::make_unique<RHI::Device>(adapterPatcher[0]);
			logger.info("Device Created.");
			logger.info("---------------------- Adapter infos ----------------------");
			logger.info("Description                   : {}", adapterPatcher[0].GetDescription());
			logger.info("DedicatedVideoMemory          : {} MB", (adapterPatcher[0].GetDedicatedVideoMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("DedicatedSystemMemory         : {} MB", (adapterPatcher[0].GetDedicatedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("SharedSystemMemory            : {} MB", (adapterPatcher[0].GetSharedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("Budget provided by OS         : {} MB", (adapterPatcher[0].GetBudget() / (1024Ui64 * 1024Ui64)));
			logger.info("Available for Reservation     : {} MB", (adapterPatcher[0].GetAvailableForReservation() / (1024Ui64 * 1024Ui64)));
			logger.info("-----------------------------------------------------------");

			logger.info("Creating Command Queues...");
			cmdQueues[utils::ToUnderlyingType(EGPUEngineType::Graphics)] = std::make_unique<RHI::CommandQueue>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			GetCommandQueue(EGPUEngineType::Graphics).SetDebugName(TEXT("Graphics"));
			cmdQueues[utils::ToUnderlyingType(EGPUEngineType::AsyncCompute)] = std::make_unique<RHI::CommandQueue>(*device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			GetCommandQueue(EGPUEngineType::AsyncCompute).SetDebugName(TEXT("AsyncCompute"));
			cmdQueues[utils::ToUnderlyingType(EGPUEngineType::Copy)] = std::make_unique<RHI::CommandQueue>(*device, D3D12_COMMAND_LIST_TYPE_COPY);
			GetCommandQueue(EGPUEngineType::Copy).SetDebugName(TEXT("Copy"));
			logger.info("Command Queues Created.");

			auto& graphicsCmdQueue = GetCommandQueue(EGPUEngineType::Graphics);

			logger.info("Creating Command List Pool...");
			cmdListPool = std::make_unique<CommandListPool>(*device, taskManager, SimultaneousFrames);
			logger.info("Command List Pool Created.");

			logger.info("Creating Descriptor Pool...");
			descriptorPool = std::make_unique<DescriptorPool>(*device, SimultaneousFrames);
			logger.info("Descriptor Pool Created.");

			logger.info("Creating Dynamic Upload Heap...");
			dynamicUploadHeap = std::make_unique<DynamicUploadHeap>(*device, InitialDynamicUploadHeapSizeInBytes, true);
			logger.info("Dynamic Upload Heap Created.");

			logger.info("Creating Swapchain...");
			swapChain = std::make_unique<SwapChain>(*device, adapterPatcher[0][0], graphicsCmdQueue, *descriptorPool, windowHandle, renderResolution, BackBufferingMode, false);
			logger.info("Swapchain Created.");

			logger.info("Creating Frame Fence...");
			frameFence = std::make_unique<RHI::FrameFence>(*device, SimultaneousFrames);
			logger.info("Frame Fence Created.");

			logger.info("Creating UI Render Context...");
			uiRenderContext = std::make_unique<UIRenderContext>(windowHandle, *device, *swapChain);
			logger.info("UI Render Context Created.");
		}

		frameFence->IncrementValue();
		NotifyFrameBegin(frameFence->GetValue());

	    auto future = taskManager.ExecuteTask(
			[&shaderFactory](const std::wstring_view path, const RHI::EShaderType type, const std::wstring_view entryPoint)
			{
				return shaderFactory.LoadShaderFromFile(path, type, entryPoint);
			}, TEXT("Resources/Shaders/CompileTest.hlsl"), RHI::EShaderType::Vertex, TEXT("main"));

		auto res = std::move(future.get());

		logger.info("RenderContext Initialized.");
		timer.Begin();
	}

	RenderContext::~RenderContext()
	{
		RHI::CommandQueue::Flush(GetCommandQueue(EGPUEngineType::Graphics), *frameFence);
		RHI::CommandQueue::Flush(GetCommandQueue(EGPUEngineType::AsyncCompute), *frameFence);
		RHI::CommandQueue::Flush(GetCommandQueue(EGPUEngineType::Copy), *frameFence);
	}

	void RenderContext::BeginFrame()
	{
		if (currentFrame > 0)
		{
			frameFence->IncrementValue();
			NotifyFrameBegin(frameFence->GetValue());
		}
	}

	void RenderContext::Render()
	{
		auto& graphicsCmdQueue = GetCommandQueue(EGPUEngineType::Graphics);
		const auto immediateCmdList = cmdListPool->Allocate<RHI::DirectCommandList>();

		/** Assume this process happen on worker thread. */
	    auto graphicsCmdList = cmdListPool->Allocate<RHI::DirectCommandList>();
		graphicsCmdList->Open();
		{
			RHI::PIXMarker marker{ *graphicsCmdList, "Render" };
			swapChain->BeginFrame(*graphicsCmdList);

			const auto clearColor = DirectX::XMFLOAT4(0.4f * std::sin(timer.GetDeltaTime()), 0.6f * std::cos(timer.GetDeltaTime()), 0.9f, 1.0f);
			swapChain->Clear(*graphicsCmdList, clearColor);
			graphicsCmdList->SetRenderTarget(swapChain->GetCurrentBackBufferRTV().Descriptor);

			uiRenderContext->Render(*graphicsCmdList);

			swapChain->EndFrame(*graphicsCmdList);
		}
		graphicsCmdList->Close();
		///////////////////////////////////////////////////////

		RefVector<RHI::CopyCommandList> pendingCmdLists;
		pendingCmdLists.emplace_back(std::ref(*graphicsCmdList));
		graphicsCmdQueue.ExecuteCommandLists(*immediateCmdList, pendingCmdLists);
		graphicsCmdList.reset();

		swapChain->Present();

		graphicsCmdQueue.Signal(*frameFence);
		frameFence->WaitForSimultaneousFramesCompletion();
	}

	void RenderContext::EndFrame()
	{
		NotifyFrameEnd(frameFence->GetCompletedValue());
		NextFrame();
	}

	void RenderContext::NotifyFrameBegin(uint64 frameNumber)
	{
		cmdListPool->BeginFrame(frameNumber);
		dynamicUploadHeap->BeginFrame(frameNumber);
		descriptorPool->BeginFrame(frameNumber);
	}

	void RenderContext::NotifyFrameEnd(uint64 completedFrameNumber)
	{
		descriptorPool->EndFrame(completedFrameNumber);
		dynamicUploadHeap->EndFrame(completedFrameNumber);
		cmdListPool->EndFrame(completedFrameNumber);
		timer.End();
	}

	void RenderContext::NextFrame()
	{
		++currentFrame;
	}
}
