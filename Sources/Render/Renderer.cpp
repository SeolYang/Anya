#include <PCH.h>
#include <Render/Renderer.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineModuleMediator.h>

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
		graphicsCommandQueue = std::make_unique<CommandQueue>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		swapChain = std::make_unique<SwapChain>(adapterPatcher[0][0], *graphicsCommandQueue, windowHandle, renderResolution, 2, false);
		backBuffersDescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, static_cast<uint32_t>(swapChain->NumBackBuffer()));
		logger.info("Renderer Initialized.");
	}

	void Renderer::Render()
	{
		//swapChain->Present();
	}
}