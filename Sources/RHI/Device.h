#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
	class Device : public RHIObject
	{
	public:
		Device(const class Adapter& adapter);

		ID3D12Device8* D3DDevice() const noexcept { return device.Get(); }
		UINT NodeMask() const noexcept { return nodeMask; }

		virtual void SetDebugName(const std::wstring_view debugName) override;

	private:
		ComPtr<ID3D12Device8> device;
		ComPtr<ID3D12InfoQueue> infoQueue;
		UINT nodeMask = 0;

	};
}