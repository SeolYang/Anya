#pragma once
#include <PCH.h>
#include <RHI/DebugLayer.h>
#include <RHI/AdapterPatcher.h>
#include <RHI/Device.h>
#include <RHI/CommandQueue.h>
#include <RHI/SwapChain.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/ResourceAllocator.h>
#include <Math/Dimension.h>

namespace sy
{
	class CommandLineParser;
	class Renderer
	{
	public:
		Renderer(HWND windowHandle, const CommandLineParser& commandLineParser);

		void Render();

	private:
		AdapterPatcher adapterPatcher;
		Dimension renderResolution;
		std::unique_ptr<Device> device;
		std::unique_ptr<ResourceAllocator> resourceAllocator;
		std::unique_ptr<CommandQueue> graphicsCommandQueue;
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<DescriptorHeap> backBuffersDescriptorHeap;

	};
}