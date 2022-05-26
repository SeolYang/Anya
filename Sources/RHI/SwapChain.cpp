#include <PCH.h>
#include <RHI/SwapChain.h>
#include <RHI/Display.h>
#include <RHI/CommandQueue.h>
#include <Core/Exceptions.h>

namespace sy
{
	SwapChain::SwapChain(const Display& display, const CommandQueue& graphicsCommandQueue, HWND windowHandle, const Dimension& surfaceDimension, uint8_t backBufferCount, bool bIsPreferHDR) :
		queue(graphicsCommandQueue),
		windowHandle(windowHandle)
	{
		uint32_t factoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		DXCall(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		DXGI_SWAP_CHAIN_DESC1 desc{
			.Width = surfaceDimension.Width,
			.Height = surfaceDimension.Height,
			.Format = ConvertColorSpaceToColorFormatForBackbuffer(display.ColorSpace(), bIsPreferHDR),
			.SampleDesc = {.Count = 1, .Quality = 0 },
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = backBufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
					(CheckFeatureSupport::PresentAllowTearing() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : (UINT)0)
		};

		ConstructSwapChain(desc);
		swapChain->SetColorSpace1(display.ColorSpace());
		SetDebugName(TEXT("SwapChain"));
	}

	void SwapChain::Present()
	{
		swapChain->Present(0, 0);
	}

	void SwapChain::ConstructSwapChain(DXGI_SWAP_CHAIN_DESC1 desc)
	{
		ComPtr<IDXGISwapChain1> _swapChain;
		DXCall(dxgiFactory->CreateSwapChainForHwnd(queue.D3DCommandQueue(), windowHandle, &desc, nullptr, nullptr, &_swapChain));
		/** Disable ALT+ENTER Full screen toggle */
		DXCall(dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));
		DXCall(_swapChain.As(&swapChain));

		backBuffers.resize(desc.BufferCount);
		for (uint32_t idx = 0; idx < desc.BufferCount; idx++)
		{
			DXCall(swapChain->GetBuffer(idx, IID_PPV_ARGS(&backBuffers[idx])));
		}
	}
}
