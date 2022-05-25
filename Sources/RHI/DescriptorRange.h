#pragma once
#include <PCH.h>

namespace sy
{
	// Descriptor Range type => SRV, UAV, CBV, SAMPLER
	class DescriptorRange
	{
	public:
		DescriptorRange() :
			range(D3D12_DESCRIPTOR_RANGE1{})
		{
		}

		inline D3D12_DESCRIPTOR_RANGE1 D3DRange() const { return range; }

	protected:
		DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32_t offsetInDescriptorsFromTableStart);

	private:
		D3D12_DESCRIPTOR_RANGE1 range;

	};

	/** Register Range => t[baseShaderRegister, baseShaderRegister+numDescriptors) */
	class DescriptorRangeSRV : public DescriptorRange
	{
	public:
		DescriptorRangeSRV(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32_t offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	};

	/** Register Range => u[baseShaderRegister, baseShaderRegister+numDescriptors) */
	class DescriptorRangeUAV : public DescriptorRange
	{
	public:
		DescriptorRangeUAV(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32_t offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	};

	/** Register Range => c[baseShaderRegister, baseShaderRegister+numDescriptors) */
	class DescriptorRangeCBV : public DescriptorRange
	{
	public:
		DescriptorRangeCBV(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32_t offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	};

	/** Register Range => s[baseShaderRegister, baseShaderRegister+numDescriptors) */
	class DescriptorRangeSampler : public DescriptorRange
	{
	public:
		DescriptorRangeSampler(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32_t offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	};

	inline auto TransformDescriptorRangeVectorTo(const std::vector<DescriptorRange>& ranges)
	{
		std::vector<D3D12_DESCRIPTOR_RANGE1> transformed;
		transformed.resize(ranges.size());

		std::transform(ranges.begin(), ranges.end(), transformed.begin(), [](const DescriptorRange& range) {return range.D3DRange(); });
		return transformed;
	}
}