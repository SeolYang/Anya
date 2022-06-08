#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
	class Device;
	class Fence;
	class CommandList;
	class CopyCommandList;
	class CommandQueue : public RHIObject
	{
	public:
		CommandQueue(const Device& device, D3D12_COMMAND_LIST_TYPE type);

		void Signal(const Fence& fence);
		void Wait(const Fence& fence);

		[[nodiscard]] ID3D12CommandQueue* GetD3DCommandQueue() const noexcept { return queue.Get(); }

		void SetDebugName(std::wstring_view debugName) override;

		static void Flush(CommandQueue& commandQueue, Fence& fence);

		void ExecuteCommandLists(CopyCommandList& immediateCmdList, std::span<Ref<CopyCommandList>> pendingCmdLists);

	private:
		ComPtr<ID3D12CommandQueue> queue;

	};
}