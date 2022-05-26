#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class RHIResource : public RHIObject
    {
    public:
        RHIResource(const ComPtr<ID3D12Resource>& existingResource);

        ID3D12Resource* Resource() const noexcept { return resource.Get(); }
        const D3D12_RESOURCE_DESC& Description() const noexcept { return resourceDesc; }

    private:
        ComPtr<ID3D12Resource> resource;
        D3D12_RESOURCE_DESC resourceDesc;

    };
}