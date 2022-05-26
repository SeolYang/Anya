#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
	class Device;
	class Fence;
	class CommandQueue : public RHIObject
	{
	public:
		CommandQueue(const Device& device, D3D12_COMMAND_LIST_TYPE type);
		virtual ~CommandQueue() { }

		void Signal(const Fence& fence);
		void Wait(const Fence& fence);

		ID3D12CommandQueue* D3DCommandQueue() const { return queue.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

	private:
		ComPtr<ID3D12CommandQueue> queue;

	};
}