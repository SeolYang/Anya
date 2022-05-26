#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class RHIResource : public RHIObject
    {
    public:
        RHIResource(const ComPtr<ID3D12Resource>& existingResource);

    private:
        ComPtr<ID3D12Resource> resource;
        D3D12_RESOURCE_DESC resourceDesc;

    };
}