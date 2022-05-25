#pragma once
#include <PCH.h>
#include <RHI/DescriptorRange.h>

// Root Constants : 0 LOI, Root Descriptors : 1 LOI, Root Descriptor Table : 2 LOI
namespace sy
{
	class RootParameter
	{
	public:
		inline D3D12_ROOT_PARAMETER1 D3DParam() const { return param; }

	protected:
		RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR_TABLE1 descriptorTable, D3D12_SHADER_VISIBILITY visibility);
		RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_CONSTANTS constants, D3D12_SHADER_VISIBILITY visibility);
		RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR1 descriptor, D3D12_SHADER_VISIBILITY visibility);

	private:
		D3D12_ROOT_PARAMETER1 param;

	};

	inline auto TransformRootParameterVectorTo(const std::vector<RootParameter>& params)
	{
		std::vector<D3D12_ROOT_PARAMETER1> transformedParams{ params.size() };
		std::transform(params.begin(), params.end(), transformedParams.begin(),
			[](const RootParameter& param)
			{
				return param.D3DParam();
			});
		return transformedParams;
	}

	class RootDescriptorTable : public RootParameter
	{
	public:
		RootDescriptorTable(D3D12_SHADER_VISIBILITY visibility, const std::vector<DescriptorRange>& ranges);
	};

	class RootConstant : public RootParameter
	{
	public:
		RootConstant(D3D12_SHADER_VISIBILITY visibility, uint32_t shaderRegister, uint32_t registerSpace = 0, uint32_t num32BitValues = 0);
	};

	class RootDescriptor : public RootParameter
	{
	public:
		// Only CBV, SRV, UAV Type for Param type!
		RootDescriptor(D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY visibility, uint32_t shaderRegister, uint32_t registerSpace = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE);
	};
}