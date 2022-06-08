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
		ANYA_ASSERT(type != D3D12_COMMAND_LIST_TYPE_BUNDLE, "Can't be create Bundle type of Command Queue.");
		const D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = type,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, /* @TODO Should i make able to customize command queue priority? */
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = device.GetNodeMask()
		};

		DXCall(device.GetD3DDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
		SetDebugName(TEXT("CommandQueue"));
	}

	void CommandQueue::Signal(const Fence& fence)
	{
		DXCall(queue->Signal(fence.GetD3DFence(), fence.GetValue()));
	}

	void CommandQueue::Wait(const Fence& fence)
	{
		DXCall(queue->Wait(fence.GetD3DFence(), fence.GetValue()));
	}

	void CommandQueue::Flush(CommandQueue& commandQueue, Fence& fence)
	{
		const HANDLE flushFenceEvent = RHI::CreateEventHandle();
		fence.IncrementValue();
		commandQueue.Signal(fence);
		fence.Wait(flushFenceEvent);
		::CloseHandle(flushFenceEvent);
	}

	void CommandQueue::ExecuteCommandLists(CopyCommandList& immediateCmdList, std::span<Ref<CopyCommandList>> pendingCmdLists)
	{
		std::vector<ID3D12CommandList*> cmdListsToExecuteOnQueue;
		for (auto& cmdListRef : pendingCmdLists)
		{
			auto& cmdList = cmdListRef.get();
			const auto resolvedBarriers = cmdList.ResolvePendingResourceBarriers();
			if (resolvedBarriers.size() > 0)
			{
				immediateCmdList.Open();
				for (const auto& barrier : resolvedBarriers)
				{
					immediateCmdList.AppendBarrierToBatch(barrier);
				}
				immediateCmdList.Close();

				cmdListsToExecuteOnQueue.emplace_back(immediateCmdList.GetD3DCommandList());
			}

			cmdListsToExecuteOnQueue.emplace_back(cmdList.GetD3DCommandList());
			queue->ExecuteCommandLists((uint32)cmdListsToExecuteOnQueue.size(), cmdListsToExecuteOnQueue.data());
			cmdListsToExecuteOnQueue.clear();
		}
	}

	void CommandQueue::SetDebugName(const std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (queue != nullptr)
		{
			DXCall(queue->SetName(debugName.data()));
		}
	}
}