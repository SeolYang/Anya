#include <PCH.h>
#include <RHI/SamplerBuilder.h.h>

namespace sy::RHI
{
    SamplerBuilder::SamplerBuilder(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressU, D3D12_TEXTURE_ADDRESS_MODE addressV, D3D12_TEXTURE_ADDRESS_MODE addressW, float32 mipLODBias, float32 minLOD, float32 maxLOD, D3D12_COMPARISON_FUNC comparisionFunc, SimpleMath::Color borderColor, uint32 maxAnisotropy) :
        sampler
            {
                .Filter = filter,
                .AddressU = addressU,
                .AddressV = addressV,
                .AddressW = addressW,
                .MipLODBias = std::clamp(mipLODBias, D3D12_MIP_LOD_BIAS_MIN, D3D12_MIP_LOD_BIAS_MAX),
                .MaxAnisotropy =  std::clamp((uint32)maxAnisotropy, (uint32)D3D12_MIN_MAXANISOTROPY, (uint32)D3D12_MAX_MAXANISOTROPY),
                .ComparisonFunc = comparisionFunc,
                .BorderColor = {borderColor.x, borderColor.y, borderColor.z, borderColor.w},
                .MinLOD = minLOD,
                .MaxLOD = maxLOD
            }
    {
    }
}
