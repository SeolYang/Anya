#pragma once
#include <PCH.h>

namespace sy
{
	class Device
	{
	public:
		Device(const class Adapter& adapter);

		ID3D12Device8* D3DDevice() const { return device.Get(); }
		UINT NodeMask() const { return nodeMask; }

	private:
		ComPtr<ID3D12Device8> device;
		UINT nodeMask = 0;

	};
}