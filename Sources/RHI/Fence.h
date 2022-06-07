#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
	class Device;
	inline HANDLE CreateEventHandle()
	{
		const auto newEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
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

		void IncrementValue() noexcept { ++value; }
		[[nodiscard]] auto GetValue() const noexcept { return value; }
		[[nodiscard]] auto GetCompletedValue() const { return fence->GetCompletedValue(); }
		void SetEventOnCompletion(uint64_t value, HANDLE event);
		void SetDebugName(const std::wstring_view debugName) override;

		/**
		* @brief Wait for itself fence value.
		*/
		void Wait(HANDLE handle);

		[[nodiscard]] ID3D12Fence1* GetD3DFence() const noexcept { return fence.Get(); }

	private:
		ComPtr<ID3D12Fence1> fence;
		uint64_t value;

	};
}