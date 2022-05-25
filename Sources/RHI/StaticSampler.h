#pragma once
#include <PCH.h>

namespace sy
{
	class StaticSampler
	{
	public:
		StaticSampler(
			D3D12_FILTER filter,
			D3D12_TEXTURE_ADDRESS_MODE addressU,
			D3D12_TEXTURE_ADDRESS_MODE addressV,
			D3D12_TEXTURE_ADDRESS_MODE addressW,
			float mipLODBias,
			uint32_t maxAnisotropy,
			D3D12_COMPARISON_FUNC comparisonFunc,
			D3D12_STATIC_BORDER_COLOR borderColor,
			float minLOD,
			float maxLOD,
			uint32_t shaderRegister,
			uint32_t registerSpace,
			D3D12_SHADER_VISIBILITY visibility);

		inline auto D3DStaticSampler() const { return samplerDesc; }

	private:
		D3D12_STATIC_SAMPLER_DESC samplerDesc;

	};

	inline auto TransformStaticSamplerVectorTo(const std::vector<StaticSampler>& samplers)
	{
		std::vector<D3D12_STATIC_SAMPLER_DESC> transformedSamplers{ samplers.size() };
		std::transform(samplers.begin(), samplers.end(), transformedSamplers.begin(),
			[](const StaticSampler& sampler)
			{
				return sampler.D3DStaticSampler();
			});

		return transformedSamplers;
	}
}
