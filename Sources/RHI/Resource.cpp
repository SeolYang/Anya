#include <PCH.h>
#include <RHI/Resource.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
    ResourceState::ResourceState(const D3D12_RESOURCE_STATES initialState, const uint32 numSubResources) :
        state(initialState),
        subResourceStates(numSubResources, initialState),
        bTrackingPerResource(numSubResources == 1)
    {
        ANYA_ASSERT(numSubResources > 0, "Subresource must be exist at least one.")
    }

    void ResourceState::SetSubResourceState(const uint32 subResourceIndex, D3D12_RESOURCE_STATES state)
    {
        ANYA_ASSERT(subResourceIndex < subResourceStates.size(), "Out of Range Sub-resource Index.");
        bTrackingPerResource == (subResourceIndex == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES || subResourceStates.size() == 1);
        if (bTrackingPerResource)
        {
            this->state = state;
        }
        else
        {
            subResourceStates[subResourceIndex] = state;
        }
    }

    Resource::Resource(const ComPtr<ID3D12Resource>& existingResource) :
        allocation(nullptr),
        resource(existingResource),
        resourceDesc(resource->GetDesc())
    {
        /**
        * @todo Implement proper method to get initial state of resource.
        */
        resourceState = ResourceState(D3D12_RESOURCE_STATE_COMMON, GetNumSubResources());
    }

    //Resource::Resource(D3D12MA::Allocator& allocator, const D3D12_RESOURCE_DESC resourceDesc) :
    //    resourceDesc(resourceDesc)
    //{
    //    D3D12MA::ALLOCATION_DESC allocDesc;
    //    allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

    //    /** How should i find proper initial resource state? */
    //    allocator.CreateResource(&allocDesc, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, &allocation, IID_PPV_ARGS(&resource));
    //}

    void Resource::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (resource != nullptr)
        {
            DXCall(resource->SetName(debugName.data()));
        }
    }
}