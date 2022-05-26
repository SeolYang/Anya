#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
	class Device;
	class DescriptorHeap : public RHIObject
	{
	public:
		DescriptorHeap(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors);

		auto D3DDescriptorHeap() const noexcept { return descriptorHeap.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

	private:
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	};
}