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

        auto GPUVirtuaAddress() const noexcept { return resource->GetGPUVirtualAddress(); }

    protected:
        /**
        * @TODO Implement Resource initialization through D3D12MA(commited) & pre-allocated heap(placed resource).
        */
        //RHIResource(D3D12MA::Allocator& allocator, D3D12_RESOURCE_DESC resourceDesc);

    private:
        ComPtr<ID3D12Resource> resource;
        ComPtr<D3D12MA::Allocation> allocation;
        D3D12_RESOURCE_DESC resourceDesc;

    };
}