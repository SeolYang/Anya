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
			logger.info("---------------------- Device infos ----------------------");
			logger.info("Description                   : {}", adapterPatcher[0].Description());
			logger.info("DedicatedVideoMemory          : {} MB", (adapterPatcher[0].DedicatedVideoMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("DedicatedSystemMemory         : {} MB", (adapterPatcher[0].DedicatedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("SharedSystemMemory            : {} MB", (adapterPatcher[0].SharedSystemMemory() / (1024Ui64 * 1024Ui64)));
			logger.info("Budget provided by OS         : {} MB", (adapterPatcher[0].Budget() / (1024Ui64 * 1024Ui64)));
			logger.info("Available for Reservation     : {} MB", (adapterPatcher[0].AvailableForReservation() / (1024Ui64 * 1024Ui64)));
			logger.info("----------------------------------------------------------");

			graphicsCmdQueue = std::make_unique<DirectCommandQueue>(*device);
			swapChain = std::make_unique<SwapChain>(*device, adapterPatcher[0][0], *graphicsCmdQueue, windowHandle, renderResolution, EBackBufferMode::Double, false);

			graphicsCmdAllocator = std::make_unique<DirectCommandAllocator>(*device);
			graphicsCmdList = std::make_unique<DirectCommandList>(*device, *graphicsCmdAllocator);

			fence = std::make_unique<Fence>(*device);
			finishEventHandle = CreateEventHandle();
		}
		logger.info("Renderer Initialized.");
	}

	Renderer::~Renderer()
	{
		CommandQueue::Flush(*graphicsCmdQueue, *fence, finishEventHandle);
		::CloseHandle(finishEventHandle);
	}

	void Renderer::Render()
	{
		graphicsCmdAllocator->Reset();
		graphicsCmdList->Reset();

		auto& backBuffer = swapChain->CurrentBackBufferTexture();
		{
			ResourceTransitionBarrier barrier{ backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET };
			graphicsCmdList->AppendResourceBarrier(barrier);

			ClearValue clearVal{ backBuffer.Format(), DirectX::XMFLOAT4(0.4f, 0.6f, 0.9f, 1.0f)};
			graphicsCmdList->ClearRenderTarget(swapChain->CurrentBackBufferRTV(), clearVal.Color);
		}

		{
			ResourceTransitionBarrier barrier{ backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT };
			graphicsCmdList->AppendResourceBarrier(barrier);
			graphicsCmdList->Close();

			graphicsCmdQueue->ExecuteCommandList(*graphicsCmdList);

			swapChain->Present();

			fence->IncrementValue();
			graphicsCmdQueue->Signal(*fence);
			fence->Wait(finishEventHandle);
		}
	}
}