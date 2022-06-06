#include <PCH.h>
#include <Rendering/RenderContext.h>
#include <Rendering/DescriptorPool.h>
#include <Rendering/DynamicUploadHeap.h>
#include <Rendering/CommandListPool.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineCoreMediator.h>
#include <RHI/DebugLayer.h>
#include <RHI/Device.h>
#include <RHI/Fence.h>
#include <RHI/FrameFence.h>
#include <RHI/Texture.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandList.h>
#include <RHI/ClearValue.h>
#include <RHI/PIXMarker.h>

namespace sy
{
	RenderContext::RenderContext(HWND windowHandle, const CommandLineParser& commandLineParser) :
		adapterPatcher(commandLineParser),
		renderResolution({ 1280, 720 }),
	    currentFrame(0)
	{
		Logger& logger = EngineCore::EngineLogger();
		const TaskManager& taskManager = EngineCore::EngineTaskManager();

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
			logger.info("Description                   : {}", adapterPatcher[0].Description());
			logger.info("DedicatedVideoMemory          : {} MB", (adapterPatcher[0].DedicatedVideoMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("DedicatedSystemMemory         : {} MB", (adapterPatcher[0].DedicatedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("SharedSystemMemory            : {} MB", (adapterPatcher[0].SharedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("Budget provided by OS         : {} MB", (adapterPatcher[0].Budget() / (1024Ui64 * 1024Ui64)));
			logger.info("Available for Reservation     : {} MB", (adapterPatcher[0].AvailableForReservation() / (1024Ui64 * 1024Ui64)));
			logger.info("-----------------------------------------------------------");

			logger.info("Creating Command Queues...");
			cmdQueues[utils::ToUnderlyingType(EGPUEngineType::Graphics)] = std::make_unique<RHI::DirectCommandQueue>(*device);
			cmdQueues[utils::ToUnderlyingType(EGPUEngineType::AsyncCompute)] = std::make_unique<RHI::ComputeCommandQueue>(*device);
			cmdQueues[utils::ToUnderlyingType(EGPUEngineType::Copy)] = std::make_unique<RHI::CopyCommandQueue>(*device);
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
		}

		frameFence->IncrementValue();
		NotifyFrameBegin(frameFence->Value());

		logger.info("RenderContext Initialized.");
		timer.Begin();
	}

	RenderContext::~RenderContext()
	{
		RHI::CommandQueue::Flush(GetCommandQueue(EGPUEngineType::Graphics), *frameFence);
		RHI::CommandQueue::Flush(GetCommandQueue(EGPUEngineType::AsyncCompute), *frameFence);
		RHI::CommandQueue::Flush(GetCommandQueue(EGPUEngineType::Copy), *frameFence);
	}

	void RenderContext::Render()
	{
		if (currentFrame > 0)
		{
			frameFence->IncrementValue();
			NotifyFrameBegin(frameFence->Value());
		}

		auto& graphicsCmdQueue = GetCommandQueue(EGPUEngineType::Graphics);
	    auto graphicsCmdList = cmdListPool->Allocate<RHI::DirectCommandList>();
		graphicsCmdList->Reset();
		auto& backBuffer = swapChain->CurrentBackBufferTexture();
		{
			RHI::PIXMarker marker{ *graphicsCmdList, "Render" };
			swapChain->BeginFrame(*graphicsCmdList);

			RHI::ClearValue clearVal{ backBuffer.Format(), };
			const auto clearColor = DirectX::XMFLOAT4(0.4f * std::sin(timer.DeltaTime()), 0.6f * std::cos(timer.DeltaTime()), 0.9f, 1.0f);
			swapChain->Clear(*graphicsCmdList, clearColor);

			swapChain->EndFrame(*graphicsCmdList);
		}
		graphicsCmdList->Close();
		graphicsCmdQueue.ExecuteCommandList(*graphicsCmdList);
		graphicsCmdList.reset();

		swapChain->Present();

		graphicsCmdQueue.Signal(*frameFence);
		frameFence->WaitForSimultaneousFramesCompletion();

		NotifyFrameEnd(frameFence->CompletedValue());
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