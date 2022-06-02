#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <Core/Timer.h>
#include <RHI/AdapterPatcher.h>
#include <RHI/SwapChain.h>

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
		class FrameFence;
		class DescriptorPool;
		class DynamicUploadHeap;
	}

	class Renderer
	{
	public:
		using DirectCommandAllocatorPtr = std::unique_ptr<RHI::DirectCommandAllocator>;
		using ComputeCommandAllocatorPtr = std::unique_ptr<RHI::ComputeCommandAllocator>;
		using CopyCommandAllocatorPtr = std::unique_ptr<RHI::CopyCommandAllocator>;
		using DirectCommandListPtr = std::unique_ptr<RHI::DirectCommandList>;
		using ComputeCommandListPtr = std::unique_ptr<RHI::ComputeCommandList>;
		using CopyCommandListPtr = std::unique_ptr<RHI::CopyCommandList>;

	public:
		Renderer(HWND windowHandle, const CommandLineParser& commandLineParser);
		~Renderer();

		void Render();
		void NotifyFrameBegin(uint64 frameNumber);
		void NotifyFrameEnd(uint64 completedFrameNumber);

	private:
		RHI::AdapterPatcher adapterPatcher;
		Dimensions renderResolution;

		constexpr static RHI::EBackBufferMode BackBufferingMode = RHI::EBackBufferMode::Double;
		constexpr static size_t SimultaneousFrames = utils::ToUnderlyingType(BackBufferingMode);
		constexpr static size_t InitialDynamicUploadHeapSizeInBytes = 65536;

		std::unique_ptr<RHI::Device> device;
		std::unique_ptr<RHI::DirectCommandQueue> graphicsCmdQueue;
		std::unique_ptr<RHI::DescriptorPool> descriptorPool;
		std::unique_ptr<RHI::DynamicUploadHeap> dynamicUploadHeap;
		std::unique_ptr<RHI::SwapChain> swapChain;
		std::vector<DirectCommandAllocatorPtr> graphicsCmdAllocators;
		std::vector<DirectCommandListPtr> graphicsCmdLists;
		std::unique_ptr<RHI::FrameFence> frameFence;

		Timer timer;

	};
}