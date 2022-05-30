#include <PCH.h>
#include <RHI/CommandQueue.h>
#include <RHI/CommandList.h>
#include <RHI/Device.h>
#include <RHI/Fence.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
	CommandQueue::CommandQueue(const Device& device, D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, /* @TODO Should i make able to customize command queue priority? */
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = device.NodeMask()
		};

		DXCall(device.D3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
		SetDebugName(TEXT("CommandQueue"));
	}

	void CommandQueue::Signal(const Fence& fence)
	{
		DXCall(queue->Signal(fence.D3DFence(), fence.Value()));
	}

	void CommandQueue::Wait(const Fence& fence)
	{
		DXCall(queue->Wait(fence.D3DFence(), fence.Value()));
	}

	void CommandQueue::Flush(CommandQueue& commandQueue, Fence& fence, HANDLE fenceEvent)
	{
		fence.IncrementValue();
		commandQueue.Signal(fence);
		fence.Wait(fenceEvent);
	}

	void CommandQueue::ExecuteCommandList(const CommandList& cmdList)
	{
		ID3D12CommandList* d3dCmdList = static_cast<ID3D12CommandList*>(cmdList.D3DCommandList());
		queue->ExecuteCommandLists(1, &d3dCmdList);
	}

	void CommandQueue::ExecuteCommandLists(const std::vector<std::reference_wrapper<const CommandList>>& cmdLists)
	{
		std::vector<ID3D12CommandList*> transformed;
		transformed.resize(cmdLists.size());
		std::transform(cmdLists.begin(), cmdLists.end(), transformed.begin(),
			[](const CommandList& cmdList)
			{
				ID3D12CommandList* d3dCmdList = static_cast<ID3D12CommandList*>(cmdList.D3DCommandList());
				return d3dCmdList;
			});

		queue->ExecuteCommandLists((uint32)transformed.size(), transformed.data());
	}

	void CommandQueue::SetDebugName(const std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (queue != nullptr)
		{
			DXCall(queue->SetName(debugName.data()));
		}
	}

	DirectCommandQueue::DirectCommandQueue(const Device& device) :
		CommandQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		SetDebugName(TEXT("DirectCommandQueue"));
	}

	BundleCommandQueue::BundleCommandQueue(const Device& device) :
		CommandQueue(device, D3D12_COMMAND_LIST_TYPE_BUNDLE)
	{
		SetDebugName(TEXT("BundleCommandQueue"));
	}

	ComputeCommandQueue::ComputeCommandQueue(const Device& device) :
		CommandQueue(device, D3D12_COMMAND_LIST_TYPE_COMPUTE)
	{
		SetDebugName(TEXT("ComputeCommandQueue"));
	}

	CopyCommandQueue::CopyCommandQueue(const Device& device) :
		CommandQueue(device, D3D12_COMMAND_LIST_TYPE_COPY)
	{
		SetDebugName(TEXT("CopyCommandQueue"));
	}
}