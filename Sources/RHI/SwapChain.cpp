#include <PCH.h>
#include <RHI/SwapChain.h>
#include <RHI/Display.h>
#include <RHI/CommandQueue.h>
#include <Core/Exceptions.h>

namespace sy
{
	SwapChain::SwapChain(const Display& display, const CommandQueue& graphicsCommandQueue, HWND windowHandle, const Dimension& surfaceDimension, uint8 backBufferCount, bool bIsPreferHDR) :
		queue(graphicsCommandQueue),
		windowHandle(windowHandle)
	{
		DXCall(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

		DXGI_SWAP_CHAIN_DESC1 desc{
			.Width = surfaceDimension.Width,
			.Height = surfaceDimension.Height,
			.Format = ConvertColorSpaceToColorFormatForBackbuffer(display.ColorSpace(), bIsPreferHDR),
			.SampleDesc = {.Count = 1, .Quality = 0 },
			.BufferUsage = DXGI_USAGE_BACK_BUFFER,
			.BufferCount = backBufferCount,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		};

		ConstructSwapChain(desc);
		swapChain->SetColorSpace1(display.ColorSpace());
	}

	void SwapChain::Present()
	{
		swapChain->Present(0, 0);
	}

	void SwapChain::ConstructSwapChain(DXGI_SWAP_CHAIN_DESC1 desc)
	{
		ComPtr<IDXGISwapChain1> _swapChain;
		DXCall(dxgiFactory->CreateSwapChainForHwnd(queue.D3DCommandQueue(), windowHandle, &desc, nullptr, nullptr, &_swapChain));
		// ThrowIfFailed(dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));
		DXCall(_swapChain.As(&swapChain));

		backBuffers.resize(desc.BufferCount);
		for (uint32_t idx = 0; idx < desc.BufferCount; idx++)
		{
			DXCall(swapChain->GetBuffer(idx, IID_PPV_ARGS(&backBuffers[idx])));
		}
	}
}
