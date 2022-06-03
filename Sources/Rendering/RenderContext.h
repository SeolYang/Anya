#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <Core/Timer.h>
#include <Core/Utility.h>
#include <RHI/AdapterPatcher.h>
#include <Rendering/SwapChain.h>

namespace sy
{
	class CommandLineParser;

	namespace RHI
	{
		class Device;
		
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
	}

	class DescriptorPool;
	class DynamicUploadHeap;
	class CommandListPool;

	class RenderContext
	{
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

		constexpr static EBufferingMode BackBufferingMode = EBufferingMode::Double;
		constexpr static size_t SimultaneousFrames = utils::ToUnderlyingType(BackBufferingMode);
		constexpr static size_t InitialDynamicUploadHeapSizeInBytes = 65536;

		std::unique_ptr<RHI::Device> device;
		std::unique_ptr<RHI::DirectCommandQueue> graphicsCmdQueue;
		std::unique_ptr<CommandListPool> cmdListPool;
		std::unique_ptr<DescriptorPool> descriptorPool;
		std::unique_ptr<DynamicUploadHeap> dynamicUploadHeap;
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<RHI::FrameFence> frameFence;

		/** Temporary Members for test */
		Timer timer;

	};
}
