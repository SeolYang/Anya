#pragma once
#include <PCH.h>
#include <RHI/Resource.h>

namespace sy::RHI
{
    class ResourceStateTracker
    {
    public:
        struct PendingResourceState
        {
            Resource* Target;
            D3D12_RESOURCE_STATES State;
            uint32 SubResourceIndex;
        };

    public:
        ResourceStateTracker() = default;
        ~ResourceStateTracker() = default;

        ResourceStateTracker(const ResourceStateTracker&) = delete;
        ResourceStateTracker(ResourceStateTracker&&) noexcept = delete;
        ResourceStateTracker& operator=(const ResourceStateTracker&) = delete;
        ResourceStateTracker& operator=(ResourceStateTracker&&) noexcept = delete;

        [[nodiscard]] ResourceState& GetResourceState(Resource* resPtr);

        void AddPendingResourceState(PendingResourceState pendingResourceState)
        {
            pendingResourceStates.emplace_back(pendingResourceState);
        }
        [[nodiscard]] std::vector<PendingResourceState>& GetPendingResourceStates() noexcept
        {
            return pendingResourceStates;
        }

        void Reset();

    private:
        robin_hood::unordered_map<Resource*, ResourceState> scopedResourceStates;
        std::vector<PendingResourceState> pendingResourceStates;

    };
}
