#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/RootParamBuilder.h>
#include <Core/Assert.h>

namespace sy::RHI
{
	class Device;
	/*
    * @TODO	Support Static Samplers too
    **/
	class RootSignatureDescBuilder
	{
	public:
		[[nodiscard]] D3D12_VERSIONED_ROOT_SIGNATURE_DESC Build() const noexcept;

		template <typename T>
		RootSignatureDescBuilder& AddRootParameter(const RootParameterBuilder<T>& builder)
		{
			ANYA_ASSERT(currentSize + builder.GetRootParameterSize() >= MaxRootSignatureSize, "Can't add more than root signature's maximum param space capacity.")
			params.emplace_back(builder.Build());
			currentSize += builder.GetRootParameterSize();
			return *this;
		}

		RootSignatureDescBuilder& ResetFlags() noexcept
		{
			flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
			return *this;
		}

		RootSignatureDescBuilder& AllowBindlessSetup() noexcept
		{
			flags |= (D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);
			return *this;
		}

		RootSignatureDescBuilder& DisallowBindlessSetup() noexcept
		{
			flags &= ~(D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);
			return *this;
		}

		RootSignatureDescBuilder& AllowInputAssemblerInputLayout() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			return *this;
		}

		RootSignatureDescBuilder& DisallowInputAssemblerInputLayout() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			return *this;
		}

		RootSignatureDescBuilder& DenyVertexShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitVertexShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& DenyHullShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitHullShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& DenyDomainShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitDomainShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& DenyGeometryShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitGeometryShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& DenyPixelShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitPixelShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AllowStreamOutput() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
			return *this;
		}

		RootSignatureDescBuilder& DisallowStreamOutput() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
			return *this;
		}

		RootSignatureDescBuilder& AsLocalRootSignature() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			return *this;
		}

		// Should i renamed it as Global Root Signature?
		RootSignatureDescBuilder& AsNonLocalRootSignature() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			return *this;
		}

		RootSignatureDescBuilder& DenyAmplificationShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitAmplificationShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& DenyMeshShaderRootAccess() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AdmitMeshShaderRootAccess() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
			return *this;
		}

		RootSignatureDescBuilder& AllowResourceHeapDirectlyIndexing() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
			return *this;
		}

		RootSignatureDescBuilder& DisallowResourceHeapDirectlyIndexing() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
			return *this;
		}

		RootSignatureDescBuilder& AllowSamplerHeapDirectlyIndexing() noexcept
		{
			flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
			return *this;
		}

		RootSignatureDescBuilder& DisallowSamplerHeapDirectlyIndexing() noexcept
		{
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
			return *this;
		}

		RootSignatureDescBuilder& SetName(std::wstring_view name)
		{
			this->name = name;
			return *this;
		}

		[[nodiscard]] std::wstring_view GetName() const noexcept
		{
			return name;
		}

	private:
		std::vector<D3D12_ROOT_PARAMETER1> params = {};
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers = {};
		constexpr static uint8 MaxRootSignatureSize = 64;
		uint8 currentSize = 0;
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED; // Bindless resource
		std::wstring name;

	};

	class RootSignature : public RHIObject
	{
	public:
		RootSignature(const Device& device, const RootSignatureDescBuilder& builder);
		/** Serialized Root Signature Blob object pointer */
		[[nodiscard]] ID3DBlob* GetD3DBlob() const { return serializedRootSignature.Get(); }
		[[nodiscard]] ID3D12RootSignature* GetD3D12RootSignature() const { return rootSignature.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

	private:
		ComPtr<ID3DBlob> serializedRootSignature;
		ComPtr<ID3D12RootSignature> rootSignature;

	};
}