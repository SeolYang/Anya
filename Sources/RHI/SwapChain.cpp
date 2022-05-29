#include <PCH.h>
#include <RHI/SwapChain.h>
#include <RHI/Display.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandList.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/Descriptor.h>
#include <RHI/Texture.h>
#include <RHI/Device.h>
#include <RHI/ResourceBarrier.h>
#include <Core/Exceptions.h>

namespace sy
{
	SwapChain::SwapChain(Device& device, const Display& display, const CommandQueue& graphicsCommandQueue, HWND windowHandle, const Dimensions& surfaceDimension, EBackBufferMode backBufferMode, bool bIsPreferHDR) :
		device(device),
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
			.BufferCount = utils::ToUnderlyingType(backBufferMode),
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
					(CheckFeatureSupport::PresentAllowTearing() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : (UINT)0)
		};

		ConstructSwapChain(desc);
		ConstructRTV(desc.BufferCount);
		swapChain->SetColorSpace1(display.ColorSpace());
		SetDebugName(TEXT("SwapChain"));
	}

	void SwapChain::Present()
	{
		swapChain->Present(0, 0);
	}

	void SwapChain::BeginFrame(CopyCommandListBase& cmdList)
	{
		ResourceTransitionBarrier barrier{ CurrentBackBufferTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET};
		cmdList.AppendResourceBarrier(barrier);
	}

	void SwapChain::EndFrame(CopyCommandListBase& cmdList)
	{
		ResourceTransitionBarrier barrier{ CurrentBackBufferTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT };
		cmdList.AppendResourceBarrier(barrier);
	}

	void SwapChain::Clear(DirectCommandListBase& cmdList, DirectX::XMFLOAT4 color)
	{
		cmdList.ClearRenderTarget(CurrentBackBufferRTV(), color);
	}

	void SwapChain::ConstructSwapChain(const DXGI_SWAP_CHAIN_DESC1 desc)
	{
		ComPtr<IDXGISwapChain1> _swapChain;
		DXCall(dxgiFactory->CreateSwapChainForHwnd(queue.D3DCommandQueue(), windowHandle, &desc, nullptr, nullptr, &_swapChain));
		/** Disable ALT+ENTER Full screen toggle */
		DXCall(dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));
		DXCall(_swapChain.As(&swapChain));
	}

	void SwapChain::ConstructRTV(const size_t bufferCount)
	{
		std::vector<ComPtr<ID3D12Resource>> backBufferResources;
		backBuffers.resize(bufferCount);
		backBufferResources.resize(bufferCount);
		for (uint32 idx = 0; idx < bufferCount; ++idx)
		{
			DXCall(swapChain->GetBuffer(idx, IID_PPV_ARGS(&backBufferResources[idx])));
			backBuffers[idx] = std::make_unique<Texture>(backBufferResources.at(idx));
		}

		backBuffersDescriptorHeap = std::make_unique<RTDescriptorHeap>(device, static_cast<uint32_t>(NumBackBuffer()));
		rtDescriptors.reserve(bufferCount);

		for (uint32 idx = 0; idx < bufferCount; ++idx)
		{
			rtDescriptors.emplace_back(backBuffersDescriptorHeap->Allocate(idx, *backBuffers.at(idx)));
		}
	}
}
