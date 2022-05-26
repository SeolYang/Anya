#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <RHI/RHI.h>
#include <RHI/RHIResource.h>

namespace sy
{
    class Texture : public RHIResource
    {
    public:
        Texture(const ComPtr<ID3D12Resource>& existingResource) :
            RHIResource(existingResource)
        {
            const auto& description = Description();
            assert(description.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER && description.Dimension == D3D12_RESOURCE_DIMENSION_UNKNOWN);
        }

        Dimensions Resolution() const
        {
            const auto& description = Description();
            return Dimensions{ description.Width, description.Height, description.DepthOrArraySize };
        }

        uint16 MipLevels() const
        {
            return Description().MipLevels;
        }

        auto Format() const
        {
            return Description().Format;
        }

        auto Dimension() const
        {
            return Description().Dimension;
        }

        uint64 Alignment() const
        {
            return Description().Alignment;
        }

        auto TextureLayout() const
        {
            return Description().Layout;
        }

    };
}