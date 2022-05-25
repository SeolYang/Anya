#pragma once
#include <PCH.h>
#include <RHI/Display.h>

namespace sy
{
	class Adapter
	{
	public:
		Adapter(const ComPtr<IDXGIAdapter1>& adapter);

		Display& operator[](size_t idx) { return displays[idx]; }
		Display operator[](size_t idx) const { return displays[idx]; }

		std::string_view Description() const { return description; }
		inline auto  VendorID() const { return vendorID; }
		inline auto DeviceID() const { return deviceID; }
		inline auto SubSysID() const { return subSysID; }
		inline auto Revision() const { return revision; }
		inline auto DedicatedVideoMemory() const { return dedicatedVideoMemory; }
		inline auto DedicatedSystemMemory() const { return dedicatedSystemMemory; }
		inline auto SharedSystemMemory() const { return sharedSystemMemory; }
		IDXGIAdapter1* D3DAdapter() const { return adapter.Get(); }

	private:
		ComPtr<IDXGIAdapter1> adapter;
		std::vector<Display> displays;

		std::string description;
		UINT vendorID;
		UINT deviceID;
		UINT subSysID;
		UINT revision;
		size_t dedicatedVideoMemory;
		size_t dedicatedSystemMemory;
		size_t sharedSystemMemory;

	};
}