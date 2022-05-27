#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class RHIResource : public RHIObject
    {
    public:
        RHIResource(const ComPtr<ID3D12Resource>& existingResource);

        virtual void SetDebugName(const std::wstring_view debugName) override;

        ID3D12Resource* D3DResource() const noexcept { return resource.Get(); }
        const D3D12_RESOURCE_DESC& Description() const noexcept { return resourceDesc; }

        auto GPUVirtualAddress() const noexcept { return resource->GetGPUVirtualAddress(); }

    private:
        ComPtr<ID3D12Resource> resource;
        D3D12_RESOURCE_DESC resourceDesc;

    };
}