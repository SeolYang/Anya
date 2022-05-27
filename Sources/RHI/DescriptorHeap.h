#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/Descriptor.h>
#include <Core/Utility.h>

namespace sy
{
	class Device;
	class Texture;
	class Buffer;
	class Sampler;
	class DescriptorHeap : public RHIObject
	{
	public:
		auto D3DDescriptorHeap() const noexcept { return descriptorHeap.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

		size_t Capacity() const noexcept { return capacity; }
		size_t DescriptorSize() const noexcept { return descriptorSize; }

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
		enum class DescriptorType : uint8_t
		{
			CBV = 0,
			SRV,
			UAV,
			NumOfTypes
		};

	public:
		CBSRUADescriptorHeap(Device& device, const uint32_t cbvCapacity, const uint32_t srvCapacity, const uint32_t uavCapacity);

	private:
		std::array<uint32_t, utils::ToUnderlyingType(DescriptorType::NumOfTypes)> descriptorCapacities;

	};

	/**
	* @brief Descriptor Heap for Sampler.
	* It will allocate on GPU (Shader Visible).
	*/
	class SamplerDescriptorHeap : public DescriptorHeap
	{
	public:
		SamplerDescriptorHeap(Device& device, const uint32_t capacity);

		SamplerDescriptor Allocate(const size_t idx, const Sampler& sampler);

	};

	/**
	* @brief Descriptor Heap for DSV(Depth-Stencil View).
	* It will allocate on CPU.
	*/
	class DSDescriptorHeap : public DescriptorHeap
	{
	public:
		DSDescriptorHeap(Device& device, const uint32_t capacity);

		DSDescriptor Allocate(const size_t idx, const Texture& texture);

	};

	/**
	* @brief Descriptor Heap for RTV(Render Target View).
	* It will allocate on CPU.
	*/
	class RTDescriptorHeap : public DescriptorHeap
	{
	public:
		RTDescriptorHeap(Device& device, const uint32_t capacity);

		RTDescriptor Allocate(const size_t idx, const Texture& texture, const uint16 mipLevel = 0);

	};
}