#include <PCH.h>
#include <RHI/ResourceAllocator.h>
#include <RHI/Device.h>
#include <RHI/Adapter.h>

namespace sy
{
	ResourceAllocator::ResourceAllocator(const Device& device, const Adapter& adapter) : 
		allocator(nullptr)
	{
		D3D12MA::ALLOCATOR_DESC desc
		{
			.pDevice = device.D3DDevice(),
			.pAdapter = adapter.D3DAdapter()
		};

		D3D12MA::CreateAllocator(&desc, &allocator);
	}

	ResourceAllocator::~ResourceAllocator()
	{
		if (allocator != nullptr)
		{
			allocator->Release();
		}
	}
}