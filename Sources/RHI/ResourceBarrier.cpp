#include <PCH.h>
#include <RHI/ResourceBarrier.h>
#include <RHI/Resource.h>


namespace sy::RHI
{
    ResourceTransitionBarrier::ResourceTransitionBarrier(const Resource& resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, uint32 subresource) :
        resource(resource)
    {
        assert(resource.D3DResource() != nullptr);
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.D3DResource(), stateBefore, stateAfter, subresource);
    }

    ResourceAliasingBarrier::ResourceAliasingBarrier(const Resource* resourceBefore, const Resource* resourceAfter) :
        resourceBefore(resourceBefore),
        resourceAfter(resourceAfter)
    {
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Aliasing(
            resourceBefore != nullptr ? resourceBefore->D3DResource() : nullptr,
            resourceAfter != nullptr ? resourceAfter->D3DResource() : nullptr);
    }

    ResourceUAVBarrier::ResourceUAVBarrier(const Resource* resource) :
        resource(resource)
    {
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::UAV(resource != nullptr ? resource->D3DResource() : nullptr);
    }
}

