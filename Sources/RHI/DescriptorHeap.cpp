#include <PCH.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy
{
	DescriptorHeap::DescriptorHeap(const Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint32_t capacity) :
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
		unitDescriptorSize = device.D3DDevice()->GetDescriptorHandleIncrementSize(heapType);
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
	
	CBSRUADescriptorHeap::CBSRUADescriptorHeap(const Device& device, const uint32_t cbvCapacity, const uint32_t srvCapacity, const uint32_t uavCapacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvCapacity + srvCapacity + uavCapacity),
		descriptorCapacities({cbvCapacity, srvCapacity, uavCapacity})
	{
	}

	SamplerDescriptorHeap::SamplerDescriptorHeap(const Device& device, const uint32_t capacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, capacity)
	{
	}

	DSDescriptorHeap::DSDescriptorHeap(const Device& device, const uint32_t capacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, capacity)
	{
	}

	RTDescriptorHeap::RTDescriptorHeap(const Device& device, const uint32_t capacity) :
		DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, capacity)
	{
	}
}