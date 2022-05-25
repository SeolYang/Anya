#pragma once
#include <PCH.h>

namespace sy
{
	class Device;
	class Adapter;
	class ResourceAllocator
	{
	public:
		ResourceAllocator(const Device& device, const Adapter& adapter);
		ResourceAllocator(const ResourceAllocator&) = delete;
		ResourceAllocator(ResourceAllocator&&) = default;

		~ResourceAllocator();

		auto Allocator() const { return allocator; }

	private:
		D3D12MA::Allocator* allocator;

	};
}