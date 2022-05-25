#pragma once
#include <PCH.h>

namespace sy
{
	class ResourceAllocator;
	class Device;
	class Resource
	{
	protected:
		Resource(const Device& device, const ResourceAllocator& allocator, D3D12_RESOURCE_DESC resourceDesc, D3D12_HEAP_TYPE heapType);

		auto D3DAllocation() const { return allocation; }
		auto D3DResource() const { return allocation->GetResource(); }

	private:
		D3D12MA::Allocation* allocation;

	};
}