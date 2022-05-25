#include <PCH.h>
#include <RHI/CommandAllocator.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy
{
	CommandAllocator::CommandAllocator(const Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		DXCall(device.D3DDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
	}
}