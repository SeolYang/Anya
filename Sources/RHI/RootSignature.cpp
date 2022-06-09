#include <PCH.h>
#include <RHI/RootSignature.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDescBuilder::Build() const noexcept
	{
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc;
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC::Init_1_1(
			desc, 
			static_cast<uint32>(params.size()), params.data(), 
			static_cast<uint32>(staticSamplers.size()), staticSamplers.data(),
			flags);

		return desc;
	}

	RootSignature::RootSignature(const Device& device, const RootSignatureDescBuilder& builder)
	{
		const D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc = builder.Build();
		DXCall(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, &serializedRootSignature, nullptr));
		DXCall(device.GetD3DDevice()->CreateRootSignature(device.GetNodeMask(), serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
		SetDebugName(builder.GetName());
	}

	void RootSignature::SetDebugName(const std::wstring_view debugName)
	{
		RHIObject::SetDebugName(debugName);
		if (rootSignature != nullptr)
		{
			rootSignature->SetName(debugName.data());
		}
	}
}