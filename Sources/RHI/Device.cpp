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

		// Debug Device?
	}
}