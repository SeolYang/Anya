#include <PCH.h>
#include <RHI/Resource.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
    Resource::Resource(const ComPtr<ID3D12Resource>& existingResource) :
        allocation(nullptr),
        resource(existingResource),
        resourceDesc(resource->GetDesc())
    {
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