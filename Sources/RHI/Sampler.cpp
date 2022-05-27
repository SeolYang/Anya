#include <PCH.h>
#include <RHI/Sampler.h>

namespace sy
{
    Sampler::Sampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressU, D3D12_TEXTURE_ADDRESS_MODE addressV, D3D12_TEXTURE_ADDRESS_MODE addressW, float32 mipLODBias, float32 minLOD, float32 maxLOD, D3D12_COMPARISON_FUNC comparisionFunc, DirectX::XMFLOAT4 borderColor, uint32 maxAnisotropy) :
        sampler(
            {
                .Filter = filter,
                .AddressU = addressU,
                .AddressV = addressV,
                .AddressW = addressW,
                .MipLODBias = mipLODBias,
                .MaxAnisotropy = maxAnisotropy,
                .ComparisonFunc = comparisionFunc,
                .BorderColor = {borderColor.x, borderColor.y, borderColor.z, borderColor.w},
                .MinLOD = minLOD,
                .MaxLOD = maxLOD
            })

    {
    }
}
