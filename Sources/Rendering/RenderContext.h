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
		class CommandListPool;
	}

	class RenderContext
	{
	public:
		using DirectCommandAllocatorPtr = std::unique_ptr<RHI::DirectCommandAllocator>;
		using ComputeCommandAllocatorPtr = std::unique_ptr<RHI::ComputeCommandAllocator>;
		using CopyCommandAllocatorPtr = std::unique_ptr<RHI::CopyCommandAllocator>;
		using DirectCommandListPtr = std::unique_ptr<RHI::DirectCommandList>;
		using ComputeCommandListPtr = std::unique_ptr<RHI::ComputeCommandList>;
		using CopyCommandListPtr = std::unique_ptr<RHI::CopyCommandList>;

	public:
		RenderContext(HWND windowHandle, const CommandLineParser& commandLineParser);
		~RenderContext();

		void Render();
		void NotifyFrameBegin(uint64 frameNumber);
		void NotifyFrameEnd(uint64 completedFrameNumber);
		void NextFrame();

	private:
		RHI::AdapterPatcher adapterPatcher;
		Dimensions renderResolution;
		size_t currentFrame;

		constexpr static RHI::EBackBufferMode BackBufferingMode = RHI::EBackBufferMode::Double;
		constexpr static size_t SimultaneousFrames = utils::ToUnderlyingType(BackBufferingMode);
		constexpr static size_t InitialDynamicUploadHeapSizeInBytes = 65536;

		std::unique_ptr<RHI::Device> device;
		std::unique_ptr<RHI::DirectCommandQueue> graphicsCmdQueue;
		std::unique_ptr<RHI::CommandListPool> cmdListPool;
		std::unique_ptr<RHI::DescriptorPool> descriptorPool;
		std::unique_ptr<RHI::DynamicUploadHeap> dynamicUploadHeap;
		std::unique_ptr<RHI::SwapChain> swapChain;
		std::unique_ptr<RHI::FrameFence> frameFence;

		/** Temporary Members for test */
		Timer timer;

	};
}