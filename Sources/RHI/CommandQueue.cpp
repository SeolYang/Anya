#include <PCH.h>
#include <RHI/CommandQueue.h>
#include <RHI/Device.h>
#include <RHI/Fence.h>
#include <Core/Exceptions.h>

namespace sy
{
	CommandQueue::CommandQueue(const Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = device.NodeMask()
		};

		DXCall(device.D3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
	}

	void CommandQueue::Signal(const Fence& fence)
	{
		DXCall(queue->Signal(fence.D3DFence(), fence.Value()));
	}

	void CommandQueue::Wait(const Fence& fence)
	{
		DXCall(queue->Wait(fence.D3DFence(), fence.Value()));
	}
}