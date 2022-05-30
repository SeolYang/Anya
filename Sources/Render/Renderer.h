#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <Core/Timer.h>
#include <RHI/AdapterPatcher.h>

namespace sy
{
	class CommandLineParser;

	namespace RHI
	{
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
	}

	using DirectCommandAllocatorPtr = std::unique_ptr<RHI::DirectCommandAllocator>;
	using ComputeCommandAllocatorPtr = std::unique_ptr<RHI::ComputeCommandAllocator>;
	using CopyCommandAllocatorPtr = std::unique_ptr<RHI::CopyCommandAllocator>;
	using DirectCommandListPtr = std::unique_ptr<RHI::DirectCommandList>;
	using ComputeCommandListPtr = std::unique_ptr<RHI::ComputeCommandList>;
	using CopyCommandListPtr = std::unique_ptr<RHI::CopyCommandList>;
	using FencePtr = std::unique_ptr<RHI::Fence>;

	class Renderer
	{
	public:
		Renderer(HWND windowHandle, const CommandLineParser& commandLineParser);
		~Renderer();

		void Render();

	private:
		RHI::AdapterPatcher adapterPatcher;
		Dimensions renderResolution;

		std::unique_ptr<RHI::Device> device;
		std::unique_ptr<RHI::DirectCommandQueue> graphicsCmdQueue;
		std::unique_ptr<RHI::SwapChain> swapChain;
		std::vector<DirectCommandAllocatorPtr> graphicsCmdAllocators;
		std::vector<DirectCommandListPtr> graphicsCmdLists;
		std::vector<FencePtr> fences;
		std::vector<HANDLE> fenceEvents;

		Timer timer;

	};
}