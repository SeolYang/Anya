#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/Descriptor.h>
#include <Core/Utility.h>

namespace sy::RHI
{
	class Device;
	class Texture;
	class Buffer;
	class SamplerBuilder;
	class DescriptorHeap : public RHIObject
	{
	public:
		auto GetD3DDescriptorHeap() const noexcept { return descriptorHeap.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

		[[nodiscard]] size_t GetCapacity() const noexcept { return capacity; }
		[[nodiscard]] size_t GetDescriptorSize() const noexcept { return descriptorSize; }

	protected:
		DescriptorHeap(Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint32_t capacity);

	protected:
		Device& device;

	private:
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		size_t descriptorSize;
		size_t capacity;

	};

	/**
	* @brief Descriptor Heap for CBV(Constant Buffer View), SRV(Shader Resource View), and UAV(Unordered Access View)
	* It will allocate on GPU (Shader Visible).
	*/
	class CBSRUADescriptorHeap : public DescriptorHeap
	{
	public:
		enum class EDescriptorType : uint8_t
		{
			ConstantBuffer = 0,
			ShaderResource,
			UnorderedAccess,
			NumOfTypes
		};

	public:
		CBSRUADescriptorHeap(Device& device, const uint32_t cbvCapacity, const uint32_t srvCapacity, const uint32_t uavCapacity);

		/**
		* @TODO	Implement CB-SR-UA Descriptor Allocations
		* [0, cb) => CB Descriptors Range => Buffer
		* [cb, cb+sr) => SR Descriptors Range => Buffer, Texture
		* [cb+sr, cb+sr+ua) => UA Descriptors Range => Buffer, Texture
		*/
		size_t IndexOf(const EDescriptorType type, const size_t idx) const noexcept;

	private:
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> OffsetOf(const EDescriptorType type, const size_t idx) const;

	private:
		std::array<uint32_t, utils::ToUnderlyingType(EDescriptorType::NumOfTypes)> descriptorCapacities;

	};

	using CBDescriptorHeap = CBSRUADescriptorHeap;
	using SRDescriptorHeap = CBSRUADescriptorHeap;
	using UADescriptorHeap = CBSRUADescriptorHeap;

	/**
	* @brief Descriptor Heap for Sampler.
	* It will allocate on GPU (Shader Visible).
	*/
	class SamplerDescriptorHeap : public DescriptorHeap
	{
	public:
		SamplerDescriptorHeap(Device& device, const uint32_t capacity);

		[[nodiscard]] SamplerDescriptor Allocate(const size_t idx, const SamplerBuilder& samplerBuilder);

	};

	/**
	* @brief Descriptor Heap for DSV(Depth-Stencil View).
	* It will allocate on CPU.
	*/
	class DSDescriptorHeap : public DescriptorHeap
	{
	public:
		DSDescriptorHeap(Device& device, const uint32_t capacity);

		[[nodiscard]] DSDescriptor Allocate(const size_t idx, const Texture& texture);

	};

	/**
	* @brief Descriptor Heap for RTV(Render Target View).
	* It will allocate on CPU.
	*/
	class RTDescriptorHeap : public DescriptorHeap
	{
	public:
		RTDescriptorHeap(Device& device, const uint32_t capacity);

		[[nodiscard]] RTDescriptor Allocate(const size_t idx, const Texture& texture, const uint16 mipLevel = 0);

	};
}