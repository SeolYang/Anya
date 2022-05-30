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

        const D3D12_SAMPLER_DESC& D3DSampler() const noexcept { return sampler; }
        D3D12_SAMPLER_DESC& D3DSampler() noexcept { return sampler; }

        auto Filter() const noexcept { return sampler.Filter; }
        auto AddressU() const noexcept { return sampler.AddressU; }
        auto AddressV() const noexcept { return sampler.AddressV; }
        auto AddressW() const noexcept { return sampler.AddressW; }
        float32 MipLODBias() const noexcept { return sampler.MipLODBias; }
        float32 MinLOD() const noexcept { return sampler.MinLOD; }
        float32 MaxLOD() const noexcept { return sampler.MaxLOD; }
        auto ComparisionFunc() const noexcept { return sampler.ComparisonFunc; }
        DirectX::XMFLOAT4 BorderColor() const noexcept { return DirectX::XMFLOAT4(sampler.BorderColor); }
        uint32 MaxAnisotropy() const noexcept { return sampler.MaxAnisotropy; }

    private:
        D3D12_SAMPLER_DESC sampler;

    };
}
