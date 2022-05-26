#include <PCH.h>
#include <Render/Renderer.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineModuleMediator.h>
#include <RHI/DebugLayer.h>
#include <RHI/Device.h>
#include <RHI/CommandQueue.h>
#include <RHI/SwapChain.h>
#include <RHI/DescriptorHeap.h>

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
		device = std::make_unique<Device>(adapterPatcher[0]);
		graphicsCommandQueue = std::make_unique<DirectCommandQueue>(*device);
		backBuffersDescriptorHeap = std::make_unique<RTDescriptorHeap>(*device, static_cast<uint32_t>(swapChain->NumBackBuffer()));
		swapChain = std::make_unique<SwapChain>(adapterPatcher[0][0], *graphicsCommandQueue, windowHandle, renderResolution, 2, false);
		logger.info("Renderer Initialized.");
	}

	void Renderer::Render()
	{
		//swapChain->Present();
	}
}