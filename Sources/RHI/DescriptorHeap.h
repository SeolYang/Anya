#pragma once
#include <PCH.h>

namespace sy
{
	class Device;
	class DescriptorHeap
	{
	public:
		DescriptorHeap(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors);

		auto D3DDescriptorHeap() const { return descriptorHeap.Get(); }

	private:
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	};
}