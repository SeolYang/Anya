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

namespace sy::RHI
{
	SwapChain::SwapChain(Device& device, const Display& display, const CommandQueue& graphicsCommandQueue, DescriptorPool& descriptorPool, HWND windowHandle, const Dimensions& surfaceDimension, EBackBufferMode backBufferMode, bool bIsPreferHDR) :
		device(device),
		descriptorPool(descriptorPool),
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

		ConstructSwapChain(desc, graphicsCommandQueue);
		swapChain->SetColorSpace1(display.ColorSpace());
		SetDebugName(TEXT("SwapChain"));
	}

	void SwapChain::Present()
	{
		swapChain->Present(0, 0);
	}

	void SwapChain::BeginFrame(CopyCommandListBase& cmdList)
	{
		for (auto& backBufferTexture : backBuffers)
		{
			rtDescriptors.emplace_back(std::move(descriptorPool.AllocateRenderTargetDescriptor(*backBufferTexture, 0)));
		}

		ResourceTransitionBarrier barrier{ CurrentBackBufferTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET};
		cmdList.AppendResourceBarrier(barrier);
	}

	void SwapChain::EndFrame(CopyCommandListBase& cmdList)
	{
		ResourceTransitionBarrier barrier{ CurrentBackBufferTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT };
		cmdList.AppendResourceBarrier(barrier);

		rtDescriptors.clear();
	}

	void SwapChain::Clear(DirectCommandListBase& cmdList, DirectX::XMFLOAT4 color)
	{
		cmdList.ClearRenderTarget(CurrentBackBufferRTV(), color);
	}

	void SwapChain::ConstructSwapChain(const DXGI_SWAP_CHAIN_DESC1 desc, const CommandQueue& cmdQueue)
	{
		ComPtr<IDXGISwapChain1> _swapChain;
		DXCall(dxgiFactory->CreateSwapChainForHwnd(cmdQueue.D3DCommandQueue(), windowHandle, &desc, nullptr, nullptr, &_swapChain));
		/** Disable ALT+ENTER Full screen toggle */
		DXCall(dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));
		DXCall(_swapChain.As(&swapChain));

		std::vector<ComPtr<ID3D12Resource>> backBufferResources;
		backBuffers.resize(desc.BufferCount);
		backBufferResources.resize(desc.BufferCount);
		for (uint32 idx = 0; idx < desc.BufferCount; ++idx)
		{
			DXCall(swapChain->GetBuffer(idx, IID_PPV_ARGS(&backBufferResources[idx])));
			backBuffers[idx] = std::make_unique<Texture>(backBufferResources.at(idx));
		}
	}
}
