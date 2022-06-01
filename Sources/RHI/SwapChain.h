#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/Descriptor.h>
#include <RHI/DescriptorPool.h>
#include <Math/Dimensions.h>

namespace sy::RHI
{
	enum class EBackBufferMode : uint8_t
	{
		None = 1, /* Single */
		Double,
		Triple
	};

	class Device;
	class Display;
	class CommandQueue;
	class DirectCommandListBase;
	class DescriptorPool;
	class CopyCommandListBase;
	class RTDescriptorHeap;
	class Texture;
	class SwapChain : public RHIObject
	{
	public:
		SwapChain(
			Device& device,
			const Display& display,
			const CommandQueue& graphicsCommandQueue,
			DescriptorPool& descriptorPool,
			HWND windowHandle,
			const Dimensions& surfaceDimension,
			EBackBufferMode backBufferMode,
			bool bIsPreferHDR);

		void Present();

		void BeginFrame(CopyCommandListBase& cmdList);
		void EndFrame(CopyCommandListBase& cmdList);
		void Clear(DirectCommandListBase& cmdList, DirectX::XMFLOAT4 color);

		const std::vector<std::unique_ptr<Texture>>& BackBuffers() const noexcept { return backBuffers; }
		auto NumBackBuffer() const noexcept { return backBuffers.size(); }
		uint64 CurrentBackBufferIndex() const { return (uint64)swapChain->GetCurrentBackBufferIndex(); }
		auto& CurrentBackBufferTexture() const { assert(CurrentBackBufferIndex() < backBuffers.size()); return *backBuffers.at(CurrentBackBufferIndex()); }
		auto CurrentBackBufferRTV() const { assert(CurrentBackBufferIndex() < rtDescriptors.size()); return *rtDescriptors.at(CurrentBackBufferIndex()); }
		IDXGISwapChain4* D3DSwapChain() const noexcept { return swapChain.Get(); }

	private:
		void ConstructSwapChain(const DXGI_SWAP_CHAIN_DESC1 desc, const CommandQueue& cmdQueue);

	private:
		Device& device;
		DescriptorPool& descriptorPool;
		ComPtr<IDXGIFactory7> dxgiFactory;
		ComPtr<IDXGISwapChain4> swapChain;
		HWND windowHandle;
		std::vector<std::unique_ptr<Texture>> backBuffers;
		std::vector<DescriptorPool::RTDescriptorPtr> rtDescriptors;

	};
}