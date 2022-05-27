#include <PCH.h>
#include <Render/Renderer.h>
#include <Core/CommandLineParser.h>
#include <Core/EngineModuleMediator.h>
#include <RHI/DebugLayer.h>
#include <RHI/Device.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandAllocator.h>
#include <RHI/CommandList.h>
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
		{
			device = std::make_unique<Device>(adapterPatcher[0]);
			graphicsCommandQueue = std::make_unique<DirectCommandQueue>(*device);
			swapChain = std::make_unique<SwapChain>(*device, adapterPatcher[0][0], *graphicsCommandQueue, windowHandle, renderResolution, 2, false);

			graphicsCommandAllocator = std::make_unique<DirectCommandAllocator>(*device);
			graphicsCommandList = std::make_unique<DirectCommandList>(*device, *graphicsCommandAllocator);
		}
		logger.info("Renderer Initialized.");
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Render()
	{
		//swapChain->Present();
	}
}