#pragma once
#include <PCH.h>

namespace sy::RHI
{
    class SamplerBuilder
    {
    public:
        SamplerBuilder(
            D3D12_FILTER filter = D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            float32 mipLODBias = 0.0f,
            float32 minLOD = 0.0f,
            float32 maxLOD = D3D12_FLOAT32_MAX,
            D3D12_COMPARISON_FUNC comparisionFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            SimpleMath::Color borderColor = SimpleMath::Color(),
            uint32 maxAnisotropy = 0) :
            sampler
            {
                .Filter = filter,
                .AddressU = addressU,
                .AddressV = addressV,
                .AddressW = addressW,
                .MipLODBias = std::clamp(mipLODBias, D3D12_MIP_LOD_BIAS_MIN, D3D12_MIP_LOD_BIAS_MAX),
                .MaxAnisotropy = std::clamp((uint32)maxAnisotropy, (uint32)D3D12_MIN_MAXANISOTROPY, (uint32)D3D12_MAX_MAXANISOTROPY),
                .ComparisonFunc = comparisionFunc,
                .BorderColor = {borderColor.x, borderColor.y, borderColor.z, borderColor.w},
                .MinLOD = minLOD,
                .MaxLOD = maxLOD
            }
        {
        }

        [[nodiscard]] D3D12_SAMPLER_DESC Build() const noexcept { return sampler; }

        [[nodiscard]] auto GetFilter() const noexcept { return sampler.Filter; }
        [[nodiscard]] auto GetAddressU() const noexcept { return sampler.AddressU; }
        [[nodiscard]] auto GetAddressV() const noexcept { return sampler.AddressV; }
        [[nodiscard]] auto GetAddressW() const noexcept { return sampler.AddressW; }
        [[nodiscard]] float32 GetMipLODBias() const noexcept { return sampler.MipLODBias; }
        [[nodiscard]] float32 GetMinLOD() const noexcept { return sampler.MinLOD; }
        [[nodiscard]] float32 GetMaxLOD() const noexcept { return sampler.MaxLOD; }
        [[nodiscard]] auto GetComparisionFunc() const noexcept { return sampler.ComparisonFunc; }
        [[nodiscard]] SimpleMath::Color GetBorderColor() const noexcept { return DirectX::XMFLOAT4(sampler.BorderColor); }
        [[nodiscard]] uint32 GetMaxAnisotropy() const noexcept { return sampler.MaxAnisotropy; }

        SamplerBuilder& SetFilter(const D3D12_FILTER filter) noexcept
        {
            sampler.Filter = filter;
            return *this;
        }

        SamplerBuilder& SetAddressModeU(const D3D12_TEXTURE_ADDRESS_MODE mode)
        {
            sampler.AddressU = mode;
            return *this;
        }

        SamplerBuilder& SetAddressModeV(const D3D12_TEXTURE_ADDRESS_MODE mode)
        {
            sampler.AddressV = mode;
            return *this;
        }

        SamplerBuilder& SetAddressModeW(const D3D12_TEXTURE_ADDRESS_MODE mode)
        {
            sampler.AddressW = mode;
            return *this;
        }

        SamplerBuilder& SetMipLODBias(const float32 mipLODBias)
        {
            sampler.MipLODBias = std::clamp(mipLODBias, D3D12_MIP_LOD_BIAS_MIN, D3D12_MIP_LOD_BIAS_MAX);
            return *this;
        }

        SamplerBuilder& SetMinLOD(const float32 minLOD)
        {
            sampler.MinLOD = minLOD;
            return *this;
        }

        SamplerBuilder& SetMaxLOD(const float32 maxLOD)
        {
            sampler.MaxLOD = maxLOD;
            return *this;
        }

        SamplerBuilder& SetComparisonFunc(const D3D12_COMPARISON_FUNC compFunc)
        {
            sampler.ComparisonFunc = compFunc;
            return *this;
        }

        SamplerBuilder& SetComparisonFunc(const SimpleMath::Color color)
        {
            sampler.BorderColor[0] = color.R();
            sampler.BorderColor[1] = color.G();
            sampler.BorderColor[2] = color.B();
            sampler.BorderColor[3] = color.A();
            return *this;
        }

        SamplerBuilder& SetMaxAnisotropic(const uint32 maxAnisotropic)
        {
            sampler.MaxAnisotropy = maxAnisotropic;
            return *this;
        }

    private:
        D3D12_SAMPLER_DESC sampler;

    };
}
