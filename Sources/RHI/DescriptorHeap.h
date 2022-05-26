#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <Core/Utility.h>

namespace sy
{
	class Device;
	class DescriptorHeap : public RHIObject
	{
	public:
		DescriptorHeap(const Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint32_t capacity);

		auto D3DDescriptorHeap() const noexcept { return descriptorHeap.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

	private:
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		size_t unitDescriptorSize;
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
		CBSRUADescriptorHeap(const Device& device, const uint32_t cbvCapacity, const uint32_t srvCapacity, const uint32_t uavCapacity);

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
		SamplerDescriptorHeap(const Device& device, const uint32_t capacity);

	};

	/**
	* @brief Descriptor Heap for DSV(Depth-Stencil View).
	* It will allocate on CPU.
	*/
	class DSDescriptorHeap : public DescriptorHeap
	{
	public:
		DSDescriptorHeap(const Device& device, const uint32_t capacity);

	};

	/**
	* @brief Descriptor Heap for RTV(Render Target View).
	* It will allocate on CPU.
	*/
	class RTDescriptorHeap : public DescriptorHeap
	{
	public:
		RTDescriptorHeap(const Device& device, const uint32_t capacity);

	};
}