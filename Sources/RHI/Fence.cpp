#include <PCH.h>
#include <RHI/Fence.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy
{
	Fence::Fence(const Device& device, uint64_t initialValue) :
		value(initialValue)
	{
		DXCall(device.D3DDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	}

	void Fence::SetEventOnCompletion(uint64_t value, HANDLE event)
	{
		DXCall (fence->SetEventOnCompletion(value, event));
	}
}