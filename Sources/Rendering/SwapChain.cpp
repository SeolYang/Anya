#include <PCH.h>
#include <Rendering/SwapChain.h>
#include <RHI/Display.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandList.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/Texture.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy
{
	SwapChain::SwapChain(RHI::Device& device, const RHI::Display& display, const RHI::CommandQueue& graphicsCommandQueue, DescriptorPool& descriptorPool, HWND windowHandle, const Dimensions& surfaceDimension, EBufferingMode backBufferMode, bool bIsPreferHDR) :
		device(device),
		descriptorPool(descriptorPool),
		windowHandle(windowHandle)
	{
		uint32_t factoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		DXCall(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory)));
		
	    desc = {
			.Width = surfaceDimension.Width,
			.Height = surfaceDimension.Height,
			.Format = RHI::ConvertColorSpaceToColorFormatForBackbuffer(display.GetColorSpace(), bIsPreferHDR),
			.SampleDesc = {.Count = 1, .Quality = 0 },
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = utils::ToUnderlyingType(backBufferMode),
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
					(RHI::CheckFeatureSupport::PresentAllowTearing() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : (UINT)0)
		};

		ConstructSwapChain(desc, graphicsCommandQueue);
		rtDescriptors.resize(backBuffers.size());
		swapChain->SetColorSpace1(display.GetColorSpace());
	}

	void SwapChain::Present()
	{
		swapChain->Present(0, 0);
	}

	void SwapChain::BeginFrame(RHI::CopyCommandList& cmdList)
	{
		rtDescriptors[GetCurrentBackBufferIndex()] = std::move(descriptorPool.AllocateRenderTargetDescriptor(GetCurrentBackBufferTexture(), 0));

		cmdList.TransitionBarrier(GetCurrentBackBufferTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void SwapChain::EndFrame(RHI::CopyCommandList& cmdList)
	{
		cmdList.TransitionBarrier(GetCurrentBackBufferTexture(), D3D12_RESOURCE_STATE_PRESENT);
		rtDescriptors[GetCurrentBackBufferIndex()].reset();
	}

	void SwapChain::Clear(RHI::DirectCommandListBase& cmdList, DirectX::XMFLOAT4 color)
	{
		cmdList.ClearRenderTarget(GetCurrentBackBufferRTV().Descriptor, color);
	}

	void SwapChain::ConstructSwapChain(const DXGI_SWAP_CHAIN_DESC1 desc, const RHI::CommandQueue& cmdQueue)
	{
		ComPtr<IDXGISwapChain1> _swapChain;
		DXCall(dxgiFactory->CreateSwapChainForHwnd(cmdQueue.GetD3DCommandQueue(), windowHandle, &desc, nullptr, nullptr, &_swapChain));
		/** Disable ALT+ENTER Full screen toggle */
		DXCall(dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));
		DXCall(_swapChain.As(&swapChain));

		std::vector<ComPtr<ID3D12Resource>> backBufferResources;
		backBuffers.resize(desc.BufferCount);
		backBufferResources.resize(desc.BufferCount);
		for (uint32 idx = 0; idx < desc.BufferCount; ++idx)
		{
			DXCall(swapChain->GetBuffer(idx, IID_PPV_ARGS(&backBufferResources[idx])));
			backBuffers[idx] = std::make_unique<RHI::Texture>(backBufferResources.at(idx));
		}
	}
}
