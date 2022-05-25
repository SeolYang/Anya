#pragma once
#include <PCH.h>
#include <RHI/CommandList.h>

namespace sy
{
	class Device;
	class CommandAllocator
	{
	public:
		CommandAllocator(const Device& device, D3D12_COMMAND_LIST_TYPE type);
		virtual ~CommandAllocator() { }

		ID3D12CommandAllocator* D3DCommandAllocator() const { return allocator.Get(); }

	private:
		ComPtr<ID3D12CommandAllocator> allocator;

	};
}