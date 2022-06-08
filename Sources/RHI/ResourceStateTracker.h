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

        [[nodiscard]] ResourceState& GetResourceState(Resource* resPtr)
        {
            ANYA_ASSERT(resPtr != nullptr, "Resource can't be a nullptr");
            ResourceState& state = scopedResourceStates[resPtr];
            if (!state.IsInitialized())
            {
                state = ResourceState(D3D12_RESOURCE_STATE_UNKNOWN, resPtr->GetNumSubResources());
            }

            return state;
        }

        void AddPendingResourceState(PendingResourceState pendingResourceState)
        {
            pendingResourceStates.emplace_back(pendingResourceState);
        }

        [[nodiscard]] const std::vector<PendingResourceState>& GetPendingResourceStates() const noexcept
        {
            return pendingResourceStates;
        }

        void Reset()
        {
            scopedResourceStates.clear();
            pendingResourceStates.clear();
        }

    private:
        robin_hood::unordered_map<Resource*, ResourceState> scopedResourceStates;
        std::vector<PendingResourceState> pendingResourceStates;

    };
}
