#include <PCH.h>
#include <RHI/RootParameter.h>

namespace sy
{
	RootParameter::RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR_TABLE1 descriptorTable, D3D12_SHADER_VISIBILITY visibility)
	{
		param = D3D12_ROOT_PARAMETER1{
			.ParameterType = type,
			.DescriptorTable = descriptorTable,
			.ShaderVisibility = visibility
		};
	}

	RootParameter::RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_CONSTANTS constants, D3D12_SHADER_VISIBILITY visibility)
	{
		param = D3D12_ROOT_PARAMETER1{
			.ParameterType = type,
			.Constants = constants,
			.ShaderVisibility = visibility
		};
	}

	RootParameter::RootParameter(D3D12_ROOT_PARAMETER_TYPE type, D3D12_ROOT_DESCRIPTOR1 descriptor, D3D12_SHADER_VISIBILITY visibility)
	{
		param = D3D12_ROOT_PARAMETER1{
			.ParameterType = type,
			.Descriptor = descriptor,
			.ShaderVisibility = visibility
		};
	}

	RootConstant::RootConstant(D3D12_SHADER_VISIBILITY visibility, uint32_t shaderRegister, uint32_t registerSpace, uint32_t num32BitValues) :
		RootParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, 
			D3D12_ROOT_CONSTANTS{
		.ShaderRegister = shaderRegister,
		.RegisterSpace = registerSpace,
		.Num32BitValues = num32BitValues
				}, 
			visibility)
	{
	}

	RootDescriptor::RootDescriptor(D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY visibility, uint32_t shaderRegister, uint32_t registerSpace, D3D12_ROOT_DESCRIPTOR_FLAGS flags) :
		RootParameter(type, 
			D3D12_ROOT_DESCRIPTOR1{
				.ShaderRegister = shaderRegister,
				.RegisterSpace = registerSpace,
				.Flags = flags
			},
			visibility)
	{
	}

	RootDescriptorTable::RootDescriptorTable(D3D12_SHADER_VISIBILITY visibility, const std::vector<DescriptorRange>& ranges) :
		RootParameter(D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, 
			D3D12_ROOT_DESCRIPTOR_TABLE1{
				.NumDescriptorRanges = static_cast<uint32_t>(ranges.size()),
				.pDescriptorRanges = (ranges.size() == 0 ? nullptr : TransformDescriptorRangeVectorTo(ranges).data())
			},
			visibility)
	{
	}
}