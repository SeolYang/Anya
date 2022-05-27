#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <RHI/AdapterPatcher.h>

namespace sy
{
	class CommandLineParser;
	class Device;
	class SwapChain;
	class CommandQueue;
	class DirectCommandQueue;
	class DirectCommandList;
	class DirectCommandAllocator;
	class CopyCommandQueue;
	class CopyCommandList;
	class CopyCommandAllocator;
	class ComputeCommandQueue;
	class ComputeCommandList;
	class ComputeCommandAllocator;
	class RTDescriptorHeap;
	class Fence;
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
		std::unique_ptr<DirectCommandQueue> graphicsCmdQueue;
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<DirectCommandAllocator> graphicsCmdAllocator;
		std::unique_ptr<DirectCommandList> graphicsCmdList;
		std::unique_ptr<Fence> fence;
		HANDLE finishEventHandle;

	};
}