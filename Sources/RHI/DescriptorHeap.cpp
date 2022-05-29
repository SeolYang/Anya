#include <PCH.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/Device.h>
#include <RHI/Texture.h>
#include <RHI/Sampler.h>
#include <Core/Exceptions.h>

namespace sy
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandleOffset(CD3DX12_CPU_DESCRIPTOR_HANDLE offsetHandle, const size_t idx, const size_t descriptorSize)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE newHandle;
		newHandle.InitOffsetted(offsetHandle, (int32)idx, (uint32)descriptorSize);
		return newHandle;
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUHandleOffset(CD3DX12_GPU_DESCRIPTOR_HANDLE offsetHandle, const size_t idx, const size_t descriptorSize)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE newHandle;
		newHandle.InitOffsetted(offsetHandle, (int32)idx, (uint32)descriptorSize);
		return newHandle;
	}

	DescriptorHeap::DescriptorHeap(Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint32_t capacity) :
		device(device),
		capacity(capacity)
	{
		// Shaer Visibility of Descriptor Heap: https://seolyang.tistory.com/35 -> 리소스 뷰 분류 참고
		bool bIsVisibleToShader = (heapType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (heapType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type = heapType,
			.NumDescriptors = capacity,
			.Flags = bIsVisibleToShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = device.NodeMask()
		};

		DXCall(device.D3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
		descriptorSize = device.D3DDevice()->GetDescriptorHandleIncrementSize(heapType);
		SetDebugName(TEXT("DescriptorHeap"));
	}

	void DescriptorHeap::SetDebugName(const std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (descriptorHeap != nullptr)
		{
			DXCall(descriptorHeap->SetName(debugName.data()));
		}
	}
	
	CBSRUADescriptorHeap::CBSRUADescriptorHeap(Device& device, const uint32_t cbvCapacity, const uint32_t srvCapacity, const uint32_t uavCapacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvCapacity + srvCapacity + uavCapacity),
		descriptorCapacities({cbvCapacity, srvCapacity, uavCapacity})
	{
		SetDebugName(TEXT("CB_SR_UA DescriporHeap"));
	}

	size_t CBSRUADescriptorHeap::IndexOf(const EDescriptorType type, const size_t idx) const noexcept
	{
		assert(type == EDescriptorType::NumOfTypes && "Invalid DescriptorType");
		assert(idx < descriptorCapacities[utils::ToUnderlyingType(type)] && "Out of Range");

		size_t actualIdx = idx;
		switch (type)
		{
		case EDescriptorType::ShaderResource:
			actualIdx += descriptorCapacities.at(0);
			break;

		case EDescriptorType::UnorderedAccess:
			actualIdx += descriptorCapacities.at(0) + descriptorCapacities.at(1);
			break;
		}

		return actualIdx;
	}

	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> CBSRUADescriptorHeap::OffsetOf(const EDescriptorType type, const size_t idx) const
	{
		const size_t actualIdx = IndexOf(type, idx);
		assert(actualIdx < Capacity() && "Out of Range");

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{ D3DDescriptorHeap()->GetCPUDescriptorHandleForHeapStart() };
		cpuHandle.Offset((int32)actualIdx, (uint32)DescriptorSize());
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{ D3DDescriptorHeap()->GetGPUDescriptorHandleForHeapStart() };
		gpuHandle.Offset((int32)actualIdx, (uint32)DescriptorSize());

		return std::make_pair(cpuHandle, gpuHandle);
	}

	SamplerDescriptorHeap::SamplerDescriptorHeap(Device& device, const uint32_t capacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, capacity)
	{
		SetDebugName(TEXT("Sampler DescriporHeap"));
	}

	SamplerDescriptor SamplerDescriptorHeap::Allocate(const size_t idx, const Sampler& sampler)
	{
		assert(idx < Capacity());

		auto heap = D3DDescriptorHeap();
		assert(heap != nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE samplerCPUHandle{ heap->GetCPUDescriptorHandleForHeapStart() };
		samplerCPUHandle = CPUHandleOffset(samplerCPUHandle, idx, DescriptorSize());
		CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle{ heap->GetGPUDescriptorHandleForHeapStart() };
		samplerGPUHandle = GPUHandleOffset(samplerGPUHandle, idx, DescriptorSize());

		device.D3DDevice()->CreateSampler(&sampler.D3DSampler(), samplerCPUHandle);

		return SamplerDescriptor{ samplerCPUHandle, samplerGPUHandle };
	}

	DSDescriptorHeap::DSDescriptorHeap(Device& device, const uint32_t capacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, capacity)
	{
		SetDebugName(TEXT("Depth-Stencil DescriporHeap"));
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC TextureToDSVDesc(const Texture& texture)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
		const auto& resolution = texture.Resolution();
		switch (texture.Dimension())
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (resolution.Depth > 1)
			{
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				desc.Texture2DArray.MipSlice = 0;
				desc.Texture2DArray.FirstArraySlice = 0;
				desc.Texture2DArray.ArraySize = resolution.Depth;
			}
			else
			{
				desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice = 0;
			}
			break;

		default:
			assert("Not supported Texture Dimension for Depth-Stencil View.");
			break;
		}

		return desc;
	}

	DSDescriptor DSDescriptorHeap::Allocate(const size_t idx, const Texture& texture)
	{
		assert(idx < Capacity());

		auto heap = D3DDescriptorHeap();
		assert(heap != nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ heap->GetCPUDescriptorHandleForHeapStart() };
		dsvHandle.Offset((int32)idx, (uint32)DescriptorSize());

		const auto dsvDesc = TextureToDSVDesc(texture);
		device.D3DDevice()->CreateDepthStencilView(texture.D3DResource(), &dsvDesc, dsvHandle);

		return dsvHandle;
	}

	RTDescriptorHeap::RTDescriptorHeap(Device& device, const uint32_t capacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, capacity)
	{
		SetDebugName(TEXT("RenderTarget DescriporHeap"));
	}

	D3D12_RENDER_TARGET_VIEW_DESC TextureToRTVDesc(const Texture& texture, const uint16 mipLevel)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

		const auto& resolution = texture.Resolution();
		switch (texture.Dimension())
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_BUFFER;
			rtvDesc.Buffer.FirstElement = 0;
			rtvDesc.Buffer.NumElements = resolution.Width;
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (resolution.Depth > 1) /* Texture1D Array */
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
				rtvDesc.Texture1DArray.MipSlice = mipLevel;
				rtvDesc.Texture1DArray.FirstArraySlice = 0;
				rtvDesc.Texture1DArray.ArraySize = resolution.Depth;
			}
			else /* Texture1D */
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
				rtvDesc.Texture1D.MipSlice = mipLevel;
			}
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (resolution.Depth > 1) /* Texture2D Array */
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.MipSlice = mipLevel;
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.PlaneSlice = 0;
				rtvDesc.Texture2DArray.ArraySize = resolution.Depth;
			}
			else /* Texture2D */
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = mipLevel;
				rtvDesc.Texture2D.PlaneSlice = 0;
			}
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtvDesc.Texture3D.MipSlice = mipLevel;
			rtvDesc.Texture3D.FirstWSlice = 0;
			rtvDesc.Texture3D.WSize = -1;
			break;

		default:
			assert("Not supported Texture Dimension for Render Target View.");
			break;
		}

		rtvDesc.Format = texture.Format();
		return rtvDesc;
	}

	RTDescriptor RTDescriptorHeap::Allocate(const size_t idx, const Texture& texture, const uint16 mipLevel)
	{
		assert(idx < Capacity());

		auto heap = D3DDescriptorHeap();
		assert(heap != nullptr);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ heap->GetCPUDescriptorHandleForHeapStart() };
		rtvHandle.Offset((int32)idx, (uint32)DescriptorSize());

		const auto rtvDesc = TextureToRTVDesc(texture, mipLevel);
		device.D3DDevice()->CreateRenderTargetView(texture.D3DResource(), &rtvDesc, rtvHandle);

		return RTDescriptor{ rtvHandle };
	}
}