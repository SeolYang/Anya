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