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
		class DirectCommandList;
		class DirectCommandAllocator;
		class CopyCommandList;
		class CopyCommandAllocator;
		class ComputeCommandAllocator;
		class ComputeCommandList;
		class RTDescriptorHeap;
		class Fence;
		class FrameFence;
	}

	class DescriptorPool;
	class DynamicUploadHeap;
	class CommandListPool;
	class UIRenderContext;
	class ShaderFactory;
	enum class EGPUEngineType : uint8
	{
	    Graphics = 0,
		AsyncCompute = 1,
		Copy = 2,
		NumOfTypes = 3
	};

	class RenderContext
	{
	public:
		RenderContext(HWND windowHandle, const CommandLineParser& commandLineParser, const ShaderFactory& shaderFactory);
		~RenderContext();

		RenderContext(const RenderContext&) = delete;
		RenderContext(RenderContext&&) noexcept = delete;
		RenderContext operator=(const RenderContext&) = delete;
		RenderContext operator=(RenderContext&&) noexcept = delete;

		void BeginFrame();
		void Render();
		void EndFrame();

		[[nodiscard]] RHI::CommandQueue& GetCommandQueue(EGPUEngineType type) const noexcept
		{
			assert(type != EGPUEngineType::NumOfTypes);
			return *cmdQueues[utils::ToUnderlyingType(type)];
		}

		[[nodiscard]] RHI::Device& GetDevice() const { return *device; }
		[[nodiscard]] SwapChain& GetSwapChain() const { return *swapChain; }
		[[nodiscard]] CommandListPool& GetCommandListPool() const { return *cmdListPool; }

	private:
		void NotifyFrameBegin(uint64 frameNumber);
		void NotifyFrameEnd(uint64 completedFrameNumber);
		void NextFrame();

	public:
		constexpr static EBufferingMode BackBufferingMode = EBufferingMode::Double;
		constexpr static size_t SimultaneousFrames = utils::ToUnderlyingType(BackBufferingMode);
		constexpr static size_t InitialDynamicUploadHeapSizeInBytes = 65536;

	private:
		RHI::AdapterPatcher adapterPatcher;
		Dimensions renderResolution;
		size_t currentFrame;

		const ShaderFactory& shaderFactory;

		std::unique_ptr<RHI::Device> device;
		std::array<std::unique_ptr<RHI::CommandQueue>, utils::ToUnderlyingType(EGPUEngineType::NumOfTypes)> cmdQueues;
		std::unique_ptr<CommandListPool> cmdListPool;
		std::unique_ptr<DescriptorPool> descriptorPool;
		std::unique_ptr<DynamicUploadHeap> dynamicUploadHeap;
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<RHI::FrameFence> frameFence;
		std::unique_ptr<UIRenderContext> uiRenderContext;

		/** Temporary Members for test */
		Timer timer;

	};
}
