#include <PCH.h>
#include <RHI/ResourceStateTracker.h>

namespace sy::RHI
{
    ResourceState& ResourceStateTracker::GetResourceState(Resource* resPtr)
    {
        ANYA_ASSERT(resPtr != nullptr, "Resource can't be a nullptr");
        ResourceState& state = scopedResourceStates[resPtr];
        if (!state.IsInitialized())
        {
            state = ResourceState(D3D12_RESOURCE_STATE_UNKNOWN, resPtr->GetNumSubResources());
        }

        return state;
    }

    void ResourceStateTracker::Reset()
    {
        scopedResourceStates.clear();
        pendingResourceStates.clear();
    }
}