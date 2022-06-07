#pragma once
#include <PCH.h>
#include <Math/Dimensions.h>
#include <RHI/RHI.h>
#include <RHI/Resource.h>

namespace sy::RHI
{
    class Texture : public Resource
    {
    public:
        Texture(const ComPtr<ID3D12Resource>& existingResource) :
            Resource(existingResource)
        {
        }

        /** @TODO   Load Texture from external file. */

        [[nodiscard]] Dimensions GetResolution() const noexcept
        {
            const auto& description = GetDescription();
            return Dimensions{ (uint32)description.Width, (uint32)description.Height, (uint32)description.DepthOrArraySize };
        }

        [[nodiscard]] uint16 MipLevels() const noexcept
        {
            return GetDescription().MipLevels;
        }

        [[nodiscard]] auto Format() const noexcept
        {
            return GetDescription().Format;
        }

        [[nodiscard]] auto Dimension() const noexcept
        {
            return GetDescription().Dimension;
        }

        [[nodiscard]] uint64 Alignment() const noexcept
        {
            return GetDescription().Alignment;
        }

        [[nodiscard]] auto TextureLayout() const noexcept
        {
            return GetDescription().Layout;
        }

    };
}