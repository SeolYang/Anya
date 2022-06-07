#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
	class Device;
	class Fence;
	class CommandList;
	class CommandQueue : public RHIObject
	{
	public:
		void Signal(const Fence& fence);
		void Wait(const Fence& fence);

		[[nodiscard]] ID3D12CommandQueue* GetD3DCommandQueue() const noexcept { return queue.Get(); }

		void SetDebugName(const std::wstring_view debugName) override;

		static void Flush(CommandQueue& commandQueue, Fence& fence);

		void ExecuteCommandList(const CommandList& cmdList);
		void ExecuteCommandLists(const std::vector <std::reference_wrapper<const CommandList>>& cmdLists);

	protected:
		CommandQueue(const Device& device, D3D12_COMMAND_LIST_TYPE type);

	private:
		ComPtr<ID3D12CommandQueue> queue;

	};

	/**
	* @brief This type of command queue can be used to execut only copy commands.
	*/
	class CopyCommandQueue : public CommandQueue
	{
	public:
		CopyCommandQueue(const Device& device);

	};

	/**
	* @brief This type of command queue can be used to execute compute and copy commands.
	*/
	class ComputeCommandQueue : public CommandQueue
	{
	public:
		ComputeCommandQueue(const Device& device);

	};

	/**
	* @brief This type of command queue can be used to execute draw, compute and copy commands.
	*/
	class DirectCommandQueue : public CommandQueue
	{
	public:
		DirectCommandQueue(const Device& device);

	};
}