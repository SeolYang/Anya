#include <PCH.h>
#include <RHI/RHIResource.h>
#include <Core/Exceptions.h>

namespace sy
{
    RHIResource::RHIResource(const ComPtr<ID3D12Resource>& existingResource) :
        resource(existingResource),
        resourceDesc(resource->GetDesc())
    {
    }

    void RHIResource::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (resource != nullptr)
        {
            DXCall(resource->SetName(debugName.data()));
        }
    }
}