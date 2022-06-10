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
		class CopyCommandList;
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

		void BeginFrame(RHI::CopyCommandList& cmdList);
		void EndFrame(RHI::CopyCommandList& cmdList);
		void Clear(RHI::DirectCommandListBase& cmdList, DirectX::XMFLOAT4 color);

		[[nodiscard]] const std::vector<std::unique_ptr<RHI::Texture>>& GetBackBuffers() const noexcept { return backBuffers; }
		[[nodiscard]] auto GetNumBackBuffer() const noexcept { return backBuffers.size(); }
		[[nodiscard]] uint64 GetCurrentBackBufferIndex() const { return (uint64)swapChain->GetCurrentBackBufferIndex(); }
		[[nodiscard]] auto& GetCurrentBackBufferTexture() const { assert(GetCurrentBackBufferIndex() < backBuffers.size()); return *backBuffers.at(GetCurrentBackBufferIndex()); }
		[[nodiscard]] auto GetCurrentBackBufferRTV() const { assert(GetCurrentBackBufferIndex() < rtDescriptors.size()); return *rtDescriptors.at(GetCurrentBackBufferIndex()); }
		[[nodiscard]] IDXGISwapChain4* GetD3DSwapChain() const noexcept { return swapChain.Get(); }
		[[nodiscard]] const DXGI_SWAP_CHAIN_DESC1& GetDesc() const noexcept { return desc; }
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
		DXGI_SWAP_CHAIN_DESC1 desc;

	};
}