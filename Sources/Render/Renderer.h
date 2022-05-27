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
	class ComputeCommandAllocator;
	class ComputeCommandQueue;
	class ComputeCommandList;
	class RTDescriptorHeap;
	class Fence;

	using DirectCommandAllocatorPtr = std::unique_ptr<DirectCommandAllocator>;
	using ComputeCommandAllocatorPtr = std::unique_ptr<ComputeCommandAllocator>;
	using CopyCommandAllocatorPtr = std::unique_ptr<CopyCommandAllocator>;
	using DirectCommandListPtr = std::unique_ptr<DirectCommandList>;
	using ComputeCommandListPtr = std::unique_ptr<ComputeCommandList>;
	using CopyCommandListPtr = std::unique_ptr<CopyCommandList>;
	using FencePtr = std::unique_ptr<Fence>;

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
		std::vector<DirectCommandAllocatorPtr> graphicsCmdAllocators;
		std::vector<DirectCommandListPtr> graphicsCmdLists;
		std::vector<FencePtr> fences;
		std::vector<HANDLE> fenceEvents;

	};
}