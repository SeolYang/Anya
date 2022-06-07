#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    class Resource : public RHIObject
    {
    public:
        Resource(const ComPtr<ID3D12Resource>& existingResource);

        virtual void SetDebugName(const std::wstring_view debugName) override;

        [[nodiscard]] ID3D12Resource* GetD3DResource() const noexcept { return resource.Get(); }
        [[nodiscard]] const D3D12_RESOURCE_DESC& GetDescription() const noexcept { return resourceDesc; }
        [[nodiscard]] auto GetGPUVirtuaAddress() const noexcept { return resource->GetGPUVirtualAddress(); }

    protected:
        /**
        * @TODO Implement Resource initialization through D3D12MA(commited) & pre-allocated heap(placed resource).
        */
        //Resource(D3D12MA::Allocator& allocator, D3D12_RESOURCE_DESC resourceDesc);

    private:
        ComPtr<ID3D12Resource> resource;
        ComPtr<D3D12MA::Allocation> allocation;
        D3D12_RESOURCE_DESC resourceDesc;

    };
}