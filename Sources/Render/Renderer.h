#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <RHI/AdapterPatcher.h>

namespace sy
{
	class CommandLineParser;
	class Device;
	class CommandQueue;
	class DirectCommandQueue;
	class SwapChain;
	class RTDescriptorHeap;
	class Renderer
	{
	public:
		Renderer(HWND windowHandle, const CommandLineParser& commandLineParser);
		~Renderer();

		void Render();

	private:
		AdapterPatcher adapterPatcher;
		Dimensions renderResolution;
		std::unique_ptr<Device> device;
		std::unique_ptr<DirectCommandQueue> graphicsCommandQueue;
		std::unique_ptr<SwapChain> swapChain;

	};
}