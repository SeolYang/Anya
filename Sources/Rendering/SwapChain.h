#pragma once
#include <PCH.h>
#include <Rendering/DescriptorPool.h>
#include <Math/Dimensions.h>

namespace sy
{
	namespace RHI
	{
		class Device;
		class Display;
		class CommandQueue;
		class DirectCommandListBase;
		class CopyCommandListBase;
		class RTDescriptorHeap;
		class Texture;
	}

	enum class EBufferingMode : uint8_t
	{
		None = 1, /* Single */
		Double,
		Triple
	};

	class SwapChain
	{
	public:
		SwapChain(
            RHI::Device& device,
			const RHI::Display& display,
			const RHI::CommandQueue& graphicsCommandQueue,
			DescriptorPool& descriptorPool,
			HWND windowHandle,
			const Dimensions& surfaceDimension,
			EBufferingMode backBufferMode,
			bool bIsPreferHDR);

		void Present();

		void BeginFrame(RHI::CopyCommandListBase& cmdList);
		void EndFrame(RHI::CopyCommandListBase& cmdList);
		void Clear(RHI::DirectCommandListBase& cmdList, DirectX::XMFLOAT4 color);

		const std::vector<std::unique_ptr<RHI::Texture>>& BackBuffers() const noexcept { return backBuffers; }
		auto NumBackBuffer() const noexcept { return backBuffers.size(); }
		uint64 CurrentBackBufferIndex() const { return (uint64)swapChain->GetCurrentBackBufferIndex(); }
		auto& CurrentBackBufferTexture() const { assert(CurrentBackBufferIndex() < backBuffers.size()); return *backBuffers.at(CurrentBackBufferIndex()); }
		auto CurrentBackBufferRTV() const { assert(CurrentBackBufferIndex() < rtDescriptors.size()); return *rtDescriptors.at(CurrentBackBufferIndex()); }
		IDXGISwapChain4* D3DSwapChain() const noexcept { return swapChain.Get(); }

	private:
		void ConstructSwapChain(const DXGI_SWAP_CHAIN_DESC1 desc, const RHI::CommandQueue& cmdQueue);

	private:
        RHI::Device& device;
		DescriptorPool& descriptorPool;
		ComPtr<IDXGIFactory7> dxgiFactory;
		ComPtr<IDXGISwapChain4> swapChain;
		HWND windowHandle;
		std::vector<std::unique_ptr<RHI::Texture>> backBuffers;
		std::vector<DescriptorPool::RTDescAllocPtr> rtDescriptors;

	};
}