#include <PCH.h>
#include <RHI/Fence.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
	Fence::Fence(const Device& device, uint64_t initialValue) :
		value(initialValue)
	{
		DXCall(device.GetD3DDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		SetDebugName(TEXT("Fence"));
	}

	void Fence::SetEventOnCompletion(uint64_t value, HANDLE event)
	{
		DXCall(fence->SetEventOnCompletion(value, event));
	}

	void Fence::SetDebugName(std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (fence != nullptr)
		{
			DXCall(fence->SetName(debugName.data()));
		}
	}

	void Fence::Wait(HANDLE handle)
	{
		constexpr std::chrono::milliseconds duration = std::chrono::milliseconds::max();
		if (GetCompletedValue() < GetValue())
		{
			SetEventOnCompletion(GetValue(), handle);
			::WaitForSingleObject(handle, (DWORD)duration.count());
		}
	}
}