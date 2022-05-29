#include <PCH.h>
#include <Render/Renderer.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineModuleMediator.h>
#include <RHI/DebugLayer.h>
#include <RHI/Device.h>
#include <RHI/Fence.h>
#include <RHI/RHIResource.h>
#include <RHI/Texture.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandAllocator.h>
#include <RHI/CommandList.h>
#include <RHI/ResourceBarrier.h>
#include <RHI/SwapChain.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/ClearValue.h>
#include <RHI/PIXMarker.h>

namespace sy
{
	Renderer::Renderer(HWND windowHandle, const CommandLineParser& commandLineParser) :
		adapterPatcher(commandLineParser),
		renderResolution({ 1280, 720 })
	{
		Logger& logger = EngineModuleMediator::LoggerModule();

		if (commandLineParser.ShouldEnableDebugLayer())
		{
			logger.info("Trying to enable DX12 debug layer...");
			DebugLayer::Enable();
			logger.info("DX12 debug layer enabled.");
		}

		logger.info("Initializing Renderer...");
		{
			device = std::make_unique<Device>(adapterPatcher[0]);
			logger.info("---------------------- Adapter infos ----------------------");
			logger.info("Description                   : {}", adapterPatcher[0].Description());
			logger.info("DedicatedVideoMemory          : {} MB", (adapterPatcher[0].DedicatedVideoMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("DedicatedSystemMemory         : {} MB", (adapterPatcher[0].DedicatedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("SharedSystemMemory            : {} MB", (adapterPatcher[0].SharedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("Budget provided by OS         : {} MB", (adapterPatcher[0].Budget() / (1024Ui64 * 1024Ui64)));
			logger.info("Available for Reservation     : {} MB", (adapterPatcher[0].AvailableForReservation() / (1024Ui64 * 1024Ui64)));
			logger.info("-----------------------------------------------------------");

			logger.info("Create Graphics Cmd Queue...");
			graphicsCmdQueue = std::make_unique<DirectCommandQueue>(*device);
			logger.info("Create Swapchain...");
			swapChain = std::make_unique<SwapChain>(*device, adapterPatcher[0][0], *graphicsCmdQueue, windowHandle, renderResolution, EBackBufferMode::Double, false);

			graphicsCmdAllocators.reserve(swapChain->NumBackBuffer());
			graphicsCmdLists.reserve(swapChain->NumBackBuffer());
			fences.reserve(swapChain->NumBackBuffer());
			fenceEvents.reserve(swapChain->NumBackBuffer());

			for (size_t idx = 0; idx < swapChain->NumBackBuffer(); ++idx)
			{
				logger.info("Initialize Cmd Allocator {}", idx);
				graphicsCmdAllocators.emplace_back(std::make_unique<DirectCommandAllocator>(*device));
				logger.info("Initialize Cmd List {}", idx);
				graphicsCmdLists.emplace_back(std::make_unique<DirectCommandList>(*device, *graphicsCmdAllocators[idx]));

				logger.info("Initialize Fence {}", idx);
				fences.emplace_back(std::make_unique<Fence>(*device));
				fenceEvents.emplace_back(CreateEventHandle());
			}

		}
		logger.info("Renderer Initialized.");
		timer.Begin();
	}

	Renderer::~Renderer()
	{
		for (size_t idx = 0; idx < fences.size(); ++idx)
		{
			CommandQueue::Flush(*graphicsCmdQueue, *fences[idx], fenceEvents[idx]);
		}

		for (auto handle : fenceEvents)
		{
			::CloseHandle(handle);
		}
	}

	void Renderer::Render()
	{
		timer.End();
		
		const auto currentBackbufferIdx = swapChain->CurrentBackBufferIndex();

		auto& graphicsCmdAllocator = *graphicsCmdAllocators[currentBackbufferIdx];
		auto& graphicsCmdList = *graphicsCmdLists[currentBackbufferIdx];
		auto& fence = *fences[currentBackbufferIdx];

		graphicsCmdAllocator.Reset();
		graphicsCmdList.Reset();

		auto& backBuffer = swapChain->CurrentBackBufferTexture();
		{
			PIXMarker marker{ graphicsCmdList, "Render" };
			swapChain->BeginFrame(graphicsCmdList);

			ClearValue clearVal{ backBuffer.Format(), };
			auto clearColor = DirectX::XMFLOAT4(0.4f * std::sin(timer.DeltaTime()), 0.6f * std::cos(timer.DeltaTime()), 0.9f, 1.0f);
			swapChain->Clear(graphicsCmdList, clearColor);

			swapChain->EndFrame(graphicsCmdList);
		}

		graphicsCmdList.Close();
		graphicsCmdQueue->ExecuteCommandList(graphicsCmdList);

		swapChain->Present();

		fence.IncrementValue();
		graphicsCmdQueue->Signal(fence);
		fence.Wait(fenceEvents[currentBackbufferIdx]);
	}
}