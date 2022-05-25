#include <PCH.h>
#include <RHI/RootSignature.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy
{
	RootSignature::RootSignature(const Device& device, const std::vector<RootParameter>& params, const std::vector<StaticSampler>& samplers, D3D12_ROOT_SIGNATURE_FLAGS flags)
	{
		D3D12_ROOT_SIGNATURE_DESC1 desc{
			.NumParameters = static_cast<uint32_t>(params.size()),
			.pParameters = (params.size() == 0 ? nullptr : TransformRootParameterVectorTo(params).data()),
			.NumStaticSamplers = static_cast<uint32_t>(samplers.size()),
			.pStaticSamplers = (samplers.size() == 0 ? nullptr : TransformStaticSamplerVectorTo(samplers).data()),
			.Flags = flags
		};

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc{
			.Desc_1_1 = desc
		};
		DXCall(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, &serializedRootSignature, nullptr));

		device.D3DDevice()->CreateRootSignature(device.NodeMask(), serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	}
}