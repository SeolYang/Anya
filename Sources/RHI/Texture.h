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
        }

        Dimensions64 Resolution() const
        {
            const auto& description = Description();
            return Dimensions64{ description.Width, description.Height, description.DepthOrArraySize };
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