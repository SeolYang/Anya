#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/Display.h>

namespace sy::RHI
{
	class Adapter : public RHIObject
	{
	public:
		Adapter(const ComPtr<IDXGIAdapter1>& adapter);

		Display& operator[](size_t idx) { return displays[idx]; }
		Display operator[](size_t idx) const { return displays[idx]; }

		std::string_view Description() const noexcept { return description; }
		inline auto  VendorID() const noexcept { return vendorID; }
		inline auto DeviceID() const noexcept { return deviceID; }
		inline auto SubSysID() const noexcept { return subSysID; }
		inline auto Revision() const noexcept { return revision; }
		inline auto DedicatedVideoMemory() const noexcept { return dedicatedVideoMemory; }
		inline auto DedicatedSystemMemory() const noexcept { return dedicatedSystemMemory; }
		inline auto SharedSystemMemory() const noexcept { return sharedSystemMemory; }

		auto Budget() const noexcept { return queriedVideoMemInfo.Budget; }
		auto CurrentUsage() const noexcept { return queriedVideoMemInfo.CurrentUsage; }
		auto AvailableForReservation() const noexcept { return queriedVideoMemInfo.AvailableForReservation; }
		auto CurrentReservation() const noexcept { return queriedVideoMemInfo.CurrentReservation; }

		IDXGIAdapter1* D3DAdapter() const noexcept { return adapter.Get(); }

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

		DXGI_QUERY_VIDEO_MEMORY_INFO queriedVideoMemInfo;

	};
}