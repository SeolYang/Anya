#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <Core/Assert.h>
#include <Math/Dimensions.h>

namespace sy::RHI
{
    class ResourceState
    {
    public:
        ResourceState() = default;
        ResourceState(const D3D12_RESOURCE_STATES initialState, const uint32 numSubResources);

        ResourceState(const ResourceState&) = default;
        ResourceState(ResourceState&&) noexcept = default;
        ResourceState& operator=(const ResourceState&) = default;
        ResourceState& operator=(ResourceState&& rhs) noexcept = default;

        [[nodiscard]] bool IsTrackingPerResource() const noexcept { return bTrackingPerResource; }
        [[nodiscard]] bool IsInitialized() const noexcept { return state != D3D12_RESOURCE_STATE_BEFORE_INITIALIZE; }
        [[nodiscard]] bool IsUnknownState() const noexcept { return state == D3D12_RESOURCE_STATE_UNKNOWN; }
        [[nodiscard]] D3D12_RESOURCE_STATES GetSubResourceState(const uint32 subResourceIndex) const
        {
            if (bTrackingPerResource)
            {
                return state;
            }

            ANYA_ASSERT(subResourceIndex < subResourceStates.size(), "Out of Range Sub-resource Index.");
            return subResourceStates[subResourceIndex];
        }
        [[nodiscard]] const auto& GetSubResourceStates() const
        {
            return subResourceStates;
        }

        void SetSubResourceState(const uint32 subResourceIndex, D3D12_RESOURCE_STATES state);

    private:
        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_BEFORE_INITIALIZE;
        std::vector<D3D12_RESOURCE_STATES> subResourceStates;
        bool bTrackingPerResource = true;

    };

    class Resource : public RHIObject
    {
    public:
        Resource(const ComPtr<ID3D12Resource>& existingResource);
        ~Resource() override = default;

        Resource(const Resource&) = delete;
        Resource(Resource&&) noexcept = delete;
        Resource& operator=(const Resource&) = delete;
        Resource& operator=(Resource&&) noexcept = delete;

        void SetDebugName(std::wstring_view debugName) override;

        [[nodiscard]] Dimensions GetResolution() const noexcept
        {
            const auto& description = GetDescription();
            return Dimensions{ (uint32)description.Width, (uint32)description.Height, (uint32)description.DepthOrArraySize };
        }
        [[nodiscard]] uint16 GetMipLevels() const noexcept { return resourceDesc.MipLevels; }
        [[nodiscard]] uint64 GetAlignment() const noexcept { return resourceDesc.Alignment; }
        [[nodiscard]] DXGI_FORMAT GetFormat() const noexcept { return resourceDesc.Format; }
        [[nodiscard]] D3D12_RESOURCE_DIMENSION GetDimension() const noexcept { return resourceDesc.Dimension; }
        [[nodiscard]] D3D12_TEXTURE_LAYOUT GetLayout() const noexcept{ return resourceDesc.Layout; }
        [[nodiscard]] D3D12_RESOURCE_DESC GetDescription() const noexcept { return resourceDesc; }
        [[nodiscard]] ID3D12Resource* GetD3DResource() const noexcept { return resource.Get(); }

        [[nodiscard]] bool IsTextureArray() const noexcept
        {
            switch (GetDimension())
            {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
                return GetResolution().Depth > 1;
            default:
                return false;
            }
        }

        [[nodiscard]] uint32 GetNumSubResources() const noexcept { return IsTextureArray() ? GetResolution().Depth * GetMipLevels() : GetMipLevels(); }
        [[nodiscard]] ResourceState& GetResourceState() noexcept { return resourceState; }
        [[nodiscard]] const ResourceState& GetResourceState() const noexcept { return resourceState; }

    protected:
        /**
        * @TODO Implement Resource initialization through D3D12MA(committed) & pre-allocated heap(placed resource).
        */
        //Resource(D3D12MA::Allocator& allocator, D3D12_RESOURCE_DESC resourceDesc);

    private:
        ComPtr<D3D12MA::Allocation> allocation;
        ComPtr<ID3D12Resource> resource;
        D3D12_RESOURCE_DESC resourceDesc;
        ResourceState resourceState;

    };
}
