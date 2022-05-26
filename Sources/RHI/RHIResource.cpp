#include <PCH.h>
#include <RHI/RHIResource.h>

namespace sy
{
    RHIResource::RHIResource(const ComPtr<ID3D12Resource>& existingResource) :
        resource(existingResource),
        resourceDesc(resource->GetDesc())
    {
    }
}