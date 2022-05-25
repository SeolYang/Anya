#include <PCH.h>
#include <RHI/DescriptorHeap.h>
#include <RHI/Device.h>

namespace sy
{
	DescriptorHeap::DescriptorHeap(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors)
	{
		bool bIsVisibleToShader = (heapType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (heapType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		D3D12_DESCRIPTOR_HEAP_DESC desc
		{
			.Type = heapType,
			.NumDescriptors = numDescriptors,
			.Flags = bIsVisibleToShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = device.NodeMask()
		};

		device.D3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
	}
}