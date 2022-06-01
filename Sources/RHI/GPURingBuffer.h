#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <Core/RingBuffer.h>

namespace sy::RHI
{
    class Device;
    class GPURingBuffer : public RHIObject
    {
    public:
        struct DynamicAllocation
        {
            ID3D12Resource* Buffer = nullptr;
            size_t Offset = 0;
            size_t Size = 0;
            void* CPUAddress = reinterpret_cast<void*>(0);
            D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = 0;
        };

    public:
        GPURingBuffer(const Device& device, const size_t maxSizeOfBuffer, const bool bAllowCPUAccess);
        GPURingBuffer(GPURingBuffer&& rhs) noexcept;
        ~GPURingBuffer() = default;

        GPURingBuffer& operator=(GPURingBuffer&&) noexcept;

        GPURingBuffer(const GPURingBuffer&) = delete;
        GPURingBuffer& operator=(const GPURingBuffer&) = delete;

        DynamicAllocation Allocate(const size_t sizeInBytes);

        virtual void SetDebugName(const std::wstring_view debugName) override;

    private:
        void Destroy();

    private:
        RingBuffer ringBuffer;
        void* cpuVirtualAddress;
        D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress;
        ComPtr<ID3D12Resource> gpuBuffer;

    };
}