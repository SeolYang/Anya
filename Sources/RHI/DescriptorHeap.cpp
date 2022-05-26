#include <PCH.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy
{
	DescriptorHeap::DescriptorHeap(const Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint32_t numDescriptors) :
		numOfDescriptors(numDescriptors)
	{
		// Shaer Visibility of Descriptor Heap: https://seolyang.tistory.com/35 -> 리소스 뷰 분류 참고
		bool bIsVisibleToShader = (heapType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (heapType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type = heapType,
			.NumDescriptors = numDescriptors,
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
			descriptorHeap->SetName(debugName.data());
		}
	}
	
	CBVSRVUAVDescriptorHeap::CBVSRVUAVDescriptorHeap(const Device& device, const uint32_t numOfDescriptors) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numOfDescriptors)
	{
	}

	SamplerDescriptorHeap::SamplerDescriptorHeap(const Device& device, const uint32_t numDescriptors) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, numDescriptors)
	{
	}

	DSVDescriptorHeap::DSVDescriptorHeap(const Device& device, const uint32_t numDescriptors) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, numDescriptors)
	{
	}

	RTVDescriptorHeap::RTVDescriptorHeap(const Device& device, const uint32_t numDescriptors) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors)
	{
	}
}