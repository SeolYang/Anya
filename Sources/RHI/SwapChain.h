#pragma once
#include <PCH.h>
#include <RHI/Device.h>
#include <Math/Dimension.h>

namespace sy
{
	class Display;
	class CommandQueue;
	class SwapChain
	{
	public:
		SwapChain(
			const Display& display,
			const CommandQueue& graphicsCommandQueue,
			HWND windowHandle,
			const Dimension& surfaceDimension,
			uint8_t backBufferCount,
			bool bIsPreferHDR);

		void Present();

		std::vector<ComPtr<ID3D12Resource2>>& BackBuffers() { return backBuffers; }
		auto NumBackBuffer() const { return backBuffers.size(); }
		IDXGISwapChain4* D3DSwapChain() const { return swapChain.Get(); }

	private:
		void ConstructSwapChain(DXGI_SWAP_CHAIN_DESC1 desc);

	private:
		ComPtr<IDXGIFactory7> dxgiFactory;
		ComPtr<IDXGISwapChain4> swapChain;
		const CommandQueue& queue;
		HWND windowHandle;
		std::vector<ComPtr<ID3D12Resource2>> backBuffers;

	};
}