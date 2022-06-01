#include <PCH.h>
#include <Rendering/Renderer.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineCoreMediator.h>
#include <RHI/DebugLayer.h>
#include <RHI/Device.h>
#include <RHI/Fence.h>
#include <RHI/Resource.h>
#include <RHI/Texture.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandAllocator.h>
#include <RHI/CommandList.h>
#include <RHI/ResourceBarrier.h>
#include <RHI/DescriptorPool.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/ClearValue.h>
#include <RHI/PIXMarker.h>

namespace sy
{
	Renderer::Renderer(HWND windowHandle, const CommandLineParser& commandLineParser) :
		adapterPatcher(commandLineParser),
		renderResolution({ 1280, 720 })
	{
		Logger& logger = EngineCore::EngineLogger();

		if (commandLineParser.ShouldEnableDebugLayer())
		{
			logger.info("Trying to enable DX12 debug layer...");
			RHI::DebugLayer::Enable();
			logger.info("DX12 debug layer enabled.");
		}

		logger.info("Initializing Renderer...");
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

			logger.info("Creating Graphics Cmd Queue...");
			graphicsCmdQueue = std::make_unique<RHI::DirectCommandQueue>(*device);
			logger.info("Graphics Cmd Queue Created.");

			logger.info("Creating Descriptor Pool...");
			descriptorPool = std::make_unique<RHI::DescriptorPool>(*device, SimultaneousFrames);
			logger.info("Descriptor Pool Created.");

			logger.info("Creating Swapchain...");
			swapChain = std::make_unique<RHI::SwapChain>(*device, adapterPatcher[0][0], *graphicsCmdQueue, *descriptorPool, windowHandle, renderResolution, BackBufferingMode, false);
			logger.info("Swapchain Created.");

			graphicsCmdAllocators.reserve(swapChain->NumBackBuffer());
			graphicsCmdLists.reserve(swapChain->NumBackBuffer());

			frameFence = std::make_unique<RHI::Fence>(*device);
			fenceEvent = RHI::CreateEventHandle();

			for (size_t idx = 0; idx < swapChain->NumBackBuffer(); ++idx)
			{
				graphicsCmdAllocators.emplace_back(std::make_unique<RHI::DirectCommandAllocator>(*device));
				graphicsCmdLists.emplace_back(std::make_unique<RHI::DirectCommandList>(*device, *graphicsCmdAllocators[idx]));
			}
		}

		frameFence->IncrementValue();
		NotifyFrameBegin(frameFence->Value());

		logger.info("Renderer Initialized.");
		timer.Begin();
	}

	Renderer::~Renderer()
	{
		RHI::CommandQueue::Flush(*graphicsCmdQueue, *frameFence, fenceEvent);
		::CloseHandle(fenceEvent);
	}

	void Renderer::Render()
	{
		const auto currentBackbufferIdx = swapChain->CurrentBackBufferIndex();

		auto& graphicsCmdAllocator = *graphicsCmdAllocators[currentBackbufferIdx];
		auto& graphicsCmdList = *graphicsCmdLists[currentBackbufferIdx];
		
		frameFence->IncrementValue();
		NotifyFrameBegin(frameFence->Value());

		graphicsCmdAllocator.Reset();
		graphicsCmdList.Reset();

		auto& backBuffer = swapChain->CurrentBackBufferTexture();
		{
			RHI::PIXMarker marker{ graphicsCmdList, "Render" };
			swapChain->BeginFrame(graphicsCmdList);

			RHI::ClearValue clearVal{ backBuffer.Format(), };
			auto clearColor = DirectX::XMFLOAT4(0.4f * std::sin(timer.DeltaTime()), 0.6f * std::cos(timer.DeltaTime()), 0.9f, 1.0f);
			swapChain->Clear(graphicsCmdList, clearColor);

			swapChain->EndFrame(graphicsCmdList);
		}
		graphicsCmdList.Close();
		graphicsCmdQueue->ExecuteCommandList(graphicsCmdList);

		swapChain->Present();

		graphicsCmdQueue->Signal(*frameFence);
		frameFence->Wait(fenceEvent);

		NotifyFrameEnd(frameFence->CompletedValue());
	}

	void Renderer::NotifyFrameBegin(uint64 frameNumber)
	{
		descriptorPool->BeginFrame(frameNumber);
	}

	void Renderer::NotifyFrameEnd(uint64 completedFrameNumber)
	{
		descriptorPool->EndFrame(completedFrameNumber);
		timer.End();
	}
}