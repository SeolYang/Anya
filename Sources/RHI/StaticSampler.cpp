#include <PCH.h>
#include <RHI/StaticSampler.h>

namespace sy
{
	StaticSampler::StaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressU, D3D12_TEXTURE_ADDRESS_MODE addressV, D3D12_TEXTURE_ADDRESS_MODE addressW, float mipLODBias, uint32_t maxAnisotropy, D3D12_COMPARISON_FUNC comparisonFunc, D3D12_STATIC_BORDER_COLOR borderColor, float minLOD, float maxLOD, uint32_t shaderRegister, uint32_t registerSpace, D3D12_SHADER_VISIBILITY visibility) :
		samplerDesc(D3D12_STATIC_SAMPLER_DESC{
		.Filter = filter,
		.AddressU = addressU,
		.AddressV = addressV,
		.AddressW = addressW,
		.MipLODBias = mipLODBias,
		.MaxAnisotropy = maxAnisotropy,
		.ComparisonFunc = comparisonFunc,
		.BorderColor = borderColor,
		.MinLOD = minLOD,
		.MaxLOD = maxLOD,
		.ShaderRegister = shaderRegister,
		.RegisterSpace = registerSpace,
		.ShaderVisibility = visibility })
	{
	}
}
