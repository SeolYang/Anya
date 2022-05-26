#include <PCH.h>
#include <RHI/Device.h>
#include <RHI/Adapter.h>
#include <Core/Exceptions.h>

namespace sy
{
	Device::Device(const Adapter& adapter)
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1;
		DXCall(D3D12CreateDevice(adapter.D3DAdapter(), featureLevel, IID_PPV_ARGS(&device)));
		SetDebugName(TEXT("Device"));
		// Debug Device?
	}

	void Device::SetDebugName(const std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (device != nullptr)
		{
			device->SetName(debugName.data());
		}
	}
}