#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
	class Device;
	class DescriptorHeap : public RHIObject
	{
	public:
		DescriptorHeap(const Device& device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint32_t numDescriptors);

		auto D3DDescriptorHeap() const noexcept { return descriptorHeap.Get(); }

		virtual void SetDebugName(const std::wstring_view debugName) override;

	private:
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		size_t descriptorSize;
		size_t numOfDescriptors;

	};

	/**
	* @brief Descriptor Heap for CBV(Constant Buffer View), SRV(Shader Resource View), and UAV(Unordered Access View)
	*/
	class CBVSRVUAVDescriptorHeap : public DescriptorHeap
	{
	public:
		CBVSRVUAVDescriptorHeap(const Device& device, const uint32_t numDescriptors);

	};

	/**
	* @brief Descriptor Heap for Sampler.
	*/
	class SamplerDescriptorHeap : public DescriptorHeap
	{
	public:
		SamplerDescriptorHeap(const Device& device, const uint32_t numDescriptors);

	};

	/**
	* @brief Descriptor Heap for DSV(Depth-Stencil View).
	*/
	class DSVDescriptorHeap : public DescriptorHeap
	{
	public:
		DSVDescriptorHeap(const Device& device, const uint32_t numDescriptors);

	};

	/**
	* @brief Descriptor Heap for RTV(Render Target View).
	*/
	class RTVDescriptorHeap : public DescriptorHeap
	{
	public:
		RTVDescriptorHeap(const Device& device, const uint32_t numDescriptors);

	};
}