#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    class Sampler : public RHIObject
    {
    public:
        Sampler(
            D3D12_FILTER filter = D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            float32 mipLODBias = 0.0f,
            float32 minLOD = 0.0f,
            float32 maxLOD = D3D12_FLOAT32_MAX,
            D3D12_COMPARISON_FUNC comparisionFunc = D3D12_COMPARISON_FUNC_ALWAYS,
            DirectX::XMFLOAT4 borderColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
            uint32 maxAnisotropy = 0);

        [[nodiscard]] const D3D12_SAMPLER_DESC& GetD3DSampler() const noexcept { return sampler; }
        [[nodiscard]] D3D12_SAMPLER_DESC& GetD3DSampler() noexcept { return sampler; }

        [[nodiscard]] auto GetFilter() const noexcept { return sampler.Filter; }
        [[nodiscard]] auto GetAddressU() const noexcept { return sampler.AddressU; }
        [[nodiscard]] auto GetAddressV() const noexcept { return sampler.AddressV; }
        [[nodiscard]] auto GetAddressW() const noexcept { return sampler.AddressW; }
        [[nodiscard]] float32 GetMipLODBias() const noexcept { return sampler.MipLODBias; }
        [[nodiscard]] float32 GetMinLOD() const noexcept { return sampler.MinLOD; }
        [[nodiscard]] float32 GetMaxLOD() const noexcept { return sampler.MaxLOD; }
        [[nodiscard]] auto GetComparisionFunc() const noexcept { return sampler.ComparisonFunc; }
        [[nodiscard]] DirectX::XMFLOAT4 GetBorderColor() const noexcept { return DirectX::XMFLOAT4(sampler.BorderColor); }
        [[nodiscard]] uint32 GetMaxAnisotropy() const noexcept { return sampler.MaxAnisotropy; }

    private:
        D3D12_SAMPLER_DESC sampler;

    };
}
