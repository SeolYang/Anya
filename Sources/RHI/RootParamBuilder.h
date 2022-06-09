#pragma once
#include <PCH.h>

namespace sy::RHI
{
	// Root Constants : 0 LOI, Root Descriptors : 1 LOI, Root Descriptor Table : 2 LOI
	template <typename T>
	class RootParameterBuilder
	{
	public:
		RootParameterBuilder() = default;
		virtual ~RootParameterBuilder() = default;

		RootParameterBuilder(const RootParameterBuilder&) = default;
		RootParameterBuilder(RootParameterBuilder&&) noexcept = default;
		RootParameterBuilder& operator=(const RootParameterBuilder&) = default;
		RootParameterBuilder& operator=(RootParameterBuilder&&) noexcept = default;

		[[nodiscard]] virtual D3D12_ROOT_PARAMETER1 Build() const noexcept = 0;
		[[nodiscard]] D3D12_SHADER_VISIBILITY GetShaderVisibility() const noexcept { return visibility; }
		[[nodiscard]] virtual uint8 GetRootParameterSize() const noexcept = 0;

		T& SetShaderVisibility(const D3D12_SHADER_VISIBILITY visibility)
		{
			this->visibility = visibility;
			return *this;
		}

	private:
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;

	};

	class RootDescriptorTableBuilder final : public RootParameterBuilder<RootDescriptorTableBuilder>
	{
	public:
		[[nodiscard]] D3D12_ROOT_PARAMETER1 Build() const noexcept override;
		[[nodiscard]] std::span<const D3D12_DESCRIPTOR_RANGE1> GetRanges() const noexcept { return ranges; }
		[[nodiscard]] virtual uint8 GetRootParameterSize() const noexcept override { return 1; }

		RootDescriptorTableBuilder& AddCBVDescriptorRange(uint32 numDescriptors = UINT_MAX, uint32 shaderRegister = 0, uint32 registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, uint32 offsetInDescriptorsFromTableStart = 0);
		RootDescriptorTableBuilder& AddSRVDescriptorRange(uint32 numDescriptors = UINT_MAX, uint32 shaderRegister = 0, uint32 registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, uint32 offsetInDescriptorsFromTableStart = 0);
		RootDescriptorTableBuilder& AddUAVDescriptorRange(uint32 numDescriptors = UINT_MAX, uint32 shaderRegister = 0, uint32 registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, uint32 offsetInDescriptorsFromTableStart = 0);
		RootDescriptorTableBuilder& AddSamplerDescriptorRange(uint32 numDescriptors = UINT_MAX, uint32 shaderRegister = 0, uint32 registerSpace = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, uint32 offsetInDescriptorsFromTableStart = 0);

	private:
		RootDescriptorTableBuilder& AddDescriptorRange(D3D12_DESCRIPTOR_RANGE1 range);

	private:
		std::vector<D3D12_DESCRIPTOR_RANGE1> ranges = {};

	};

	class RootDescriptorBuilder final : public RootParameterBuilder<RootDescriptorBuilder>
	{
	public:
		[[nodiscard]] D3D12_ROOT_PARAMETER1 Build() const noexcept override;
		[[nodiscard]] virtual uint8 GetRootParameterSize() const noexcept override { return 2; }

		RootDescriptorBuilder& InitAsSRV()
		{
			type = D3D12_ROOT_PARAMETER_TYPE_SRV;
			return *this;
		}

		RootDescriptorBuilder& InitAsCBV()
		{
			type = D3D12_ROOT_PARAMETER_TYPE_CBV;
			return *this;
		}

		RootDescriptorBuilder& InitAsUAV()
		{
			type = D3D12_ROOT_PARAMETER_TYPE_UAV;
			return *this;
		}

		RootDescriptorBuilder& SetShaderRegister(const uint32 shaderRegister = 0) noexcept
		{
			this->shaderRegister = shaderRegister;
			return *this;
		}

		RootDescriptorBuilder& SetRegisterSpace(const uint32 registerSpace = 0) noexcept
		{
			this->registerSpace = registerSpace;
			return *this;
		}

		RootDescriptorBuilder& ResetFlags() noexcept
		{
			flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
			return *this;
		}

		RootDescriptorBuilder& MarkAsDataVolatile() noexcept
		{
			flags |= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
			return *this;
		}

		RootDescriptorBuilder& UnmarkDataVolatile() noexcept
		{
			flags &= ~D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
			return *this;
		}

		RootDescriptorBuilder& MarkAsDataStaticWhileSetAtExecute() noexcept
		{
			flags |= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
			return *this;
		}

		RootDescriptorBuilder& UnmarkDataStaticWhileSetAtExecute() noexcept
		{
			flags &= ~D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
			return *this;
		}

		RootDescriptorBuilder& MarkAsDataStatic() noexcept
		{
			flags |= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;
			return *this;
		}

		RootDescriptorBuilder& UnmarkDataStatic() noexcept
		{
			flags &= ~D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;
			return *this;
		}

	private:
		D3D12_ROOT_PARAMETER_TYPE type = D3D12_ROOT_PARAMETER_TYPE_SRV;
		D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		uint32 shaderRegister = 0;
		uint32 registerSpace = 0;

	};

	class RootConstantBuilder final : public RootParameterBuilder<RootConstantBuilder>
	{
	public:
		[[nodiscard]] D3D12_ROOT_PARAMETER1 Build() const noexcept override;

		RootConstantBuilder& SetShaderRegister(const uint32 shaderRegister = 0) noexcept
		{
			this->shaderRegister = shaderRegister;
			return *this;
		}

		RootConstantBuilder& SetRegisterSpace(const uint32 registerSpace = 0) noexcept
		{
			this->registerSpace = registerSpace;
			return *this;
		}

		RootConstantBuilder& SetNum32BitValues(const uint32 num32BitValues = 0) noexcept
		{
			this->num32BitValues = num32BitValues;
			return *this;
		}

		[[nodiscard]] virtual uint8 GetRootParameterSize() const noexcept override { return 1; }

	private:
		uint32 shaderRegister = 0;
		uint32 registerSpace = 0;
		uint32 num32BitValues = 0;

	};
}