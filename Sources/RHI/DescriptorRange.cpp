#include <PCH.h>
#include <RHI/DescriptorRange.h>

namespace sy
{
	DescriptorRange::DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32_t offsetInDescriptorsFromTableStart) :
		range(D3D12_DESCRIPTOR_RANGE1{
				.RangeType = rangeType,
				.NumDescriptors = numDescriptors,
				.BaseShaderRegister = baseShaderRegister,
				.RegisterSpace = registerSpace,
				.Flags = flags,
				.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart
			})
	{
	}

	DescriptorRangeSRV::DescriptorRangeSRV(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32_t offsetInDescriptorsFromTableStart) :
		DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, baseShaderRegister, registerSpace,
			flags, offsetInDescriptorsFromTableStart)
	{
	}


	DescriptorRangeUAV::DescriptorRangeUAV(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32_t offsetInDescriptorsFromTableStart) :
		DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, numDescriptors, baseShaderRegister, registerSpace,
			flags, offsetInDescriptorsFromTableStart)
	{
	}


	DescriptorRangeCBV::DescriptorRangeCBV(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32_t offsetInDescriptorsFromTableStart) :
		DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, baseShaderRegister, registerSpace,
			flags, offsetInDescriptorsFromTableStart)
	{
	}


	DescriptorRangeSampler::DescriptorRangeSampler(uint32_t numDescriptors, uint32_t baseShaderRegister, uint32_t registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32_t offsetInDescriptorsFromTableStart) :
		DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, numDescriptors, baseShaderRegister, registerSpace,
			flags, offsetInDescriptorsFromTableStart)
	{
	}
}