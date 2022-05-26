#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <Core/Exceptions.h>

namespace sy
{
	class Device;
	inline HANDLE CreateEventHandle()
	{
		auto newEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (newEvent == nullptr)
		{
			Win32Call(HRESULT_FROM_WIN32(GetLastError()));
		}

		return newEvent;
	}

	class Fence : public RHIObject
	{
	public:
		Fence(const Device& device, uint64_t initialValue = 0);

		inline void IncrementValue() noexcept { ++value; }
		inline auto Value() const noexcept { return value; }
		inline auto CompletedValue() const { return fence->GetCompletedValue(); }
		void SetEventOnCompletion(uint64_t value, HANDLE event);
		virtual void SetDebugName(const std::wstring_view debugName) override;

		ID3D12Fence1* D3DFence() const noexcept { return fence.Get(); }

	private:
		ComPtr<ID3D12Fence1> fence;
		uint64_t value;

	};
}