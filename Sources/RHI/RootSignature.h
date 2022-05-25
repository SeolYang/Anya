#pragma once
#include <PCH.h>
#include <RHI/RootParameter.h>
#include <RHI/StaticSampler.h>

namespace sy
{
	class RootSignature
	{
	public:
		RootSignature(const class Device& device, const std::vector<RootParameter>& params, const std::vector<StaticSampler>& samplers, D3D12_ROOT_SIGNATURE_FLAGS flags);

		/** Serialized Root Signature Blob object pointer */
		ID3DBlob* D3DBlob() const { return serializedRootSignature.Get(); }
		ID3D12RootSignature* D3D12RootSignature() const { return rootSignature.Get(); }

	private:
		ComPtr<ID3DBlob> serializedRootSignature;
		ComPtr<ID3D12RootSignature> rootSignature;

	};
}