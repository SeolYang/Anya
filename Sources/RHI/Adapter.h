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

        std::string_view GetDescription() const noexcept { return description; }
        [[nodiscard]] auto GetVendorID() const noexcept { return vendorID; }
        [[nodiscard]] auto GetDeviceID() const noexcept { return deviceID; }
        [[nodiscard]] auto GetSubSysID() const noexcept { return subSysID; }
        [[nodiscard]] auto GetRevision() const noexcept { return revision; }
        [[nodiscard]] auto GetDedicatedVideoMemory() const noexcept { return dedicatedVideoMemory; }
        [[nodiscard]] auto GetDedicatedSystemMemory() const noexcept { return dedicatedSystemMemory; }
        [[nodiscard]] auto GetSharedSystemMemory() const noexcept { return sharedSystemMemory; }

        [[nodiscard]] auto GetBudget() const noexcept { return queriedVideoMemInfo.Budget; }
        [[nodiscard]] auto GetCurrentUsage() const noexcept { return queriedVideoMemInfo.CurrentUsage; }
        [[nodiscard]] auto GetAvailableForReservation() const noexcept { return queriedVideoMemInfo.AvailableForReservation; }
        [[nodiscard]] auto GetCurrentReservation() const noexcept { return queriedVideoMemInfo.CurrentReservation; }

        [[nodiscard]] IDXGIAdapter1* GetD3DAdapter() const noexcept { return adapter.Get(); }

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