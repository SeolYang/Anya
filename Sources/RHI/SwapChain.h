#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/Descriptor.h>
#include <Math/Dimensions.h>

namespace sy
{
	class Device;
	class Display;
	class CommandQueue;
	class RTDescriptorHeap;
	class Texture;
	class SwapChain : public RHIObject
	{
	public:
		SwapChain(
			Device& device,
			const Display& display,
			const CommandQueue& graphicsCommandQueue,
			HWND windowHandle,
			const Dimensions& surfaceDimension,
			uint8_t backBufferCount,
			bool bIsPreferHDR);

		void Present();

		const std::vector<std::unique_ptr<Texture>>& BackBuffers() const noexcept { return backBuffers; }
		const std::vector<RTDescriptor>& RTDescriptors() const noexcept { return rtDescriptors; }
		auto NumBackBuffer() const noexcept { return backBuffers.size(); }
		IDXGISwapChain4* D3DSwapChain() const noexcept { return swapChain.Get(); }

	private:
		void ConstructSwapChain(const DXGI_SWAP_CHAIN_DESC1 desc);
		void ConstructRTV(const size_t bufferCount);

	private:
		Device& device;
		ComPtr<IDXGIFactory7> dxgiFactory;
		ComPtr<IDXGISwapChain4> swapChain;
		const CommandQueue& queue;
		HWND windowHandle;
		std::vector<std::unique_ptr<Texture>> backBuffers;
		std::vector<RTDescriptor> rtDescriptors;
		std::unique_ptr<RTDescriptorHeap> backBuffersDescriptorHeap;

	};
}