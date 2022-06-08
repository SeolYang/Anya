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
            void* CPUAddress = nullptr;
            D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = 0;
        };

    public:
        GPURingBuffer(const Device& device, size_t maxSizeOfBuffer, bool bAllowCPUAccess);
        GPURingBuffer(GPURingBuffer&& rhs) noexcept;
        ~GPURingBuffer() override = default;

        GPURingBuffer& operator=(GPURingBuffer&&) noexcept;

        GPURingBuffer(const GPURingBuffer&) = delete;
        GPURingBuffer& operator=(const GPURingBuffer&) = delete;

        [[nodiscard]] DynamicAllocation Allocate(const size_t sizeInBytes);

        void FinishCurrentFrame(const uint64 currentFrameFenceValue)
        {
            ringBuffer.FinishCurrentFrame(currentFrameFenceValue);
        }

        void ReleaseCompletedFrame(const uint64 completedFenceValue)
        {
            ringBuffer.ReleaseCompletedFrame(completedFenceValue);
        }

        void SetReleaseCompletedFrameCallback(const RingBuffer::ReleaseCompletedFrameCallback& callback)
        {
            ringBuffer.SetReleaseCompletedFrameCallback(callback);
        }

        [[nodiscard]] size_t GetMaxSize() const noexcept { return ringBuffer.MaxSize(); }
        [[nodiscard]] bool IsFull() const noexcept { return ringBuffer.IsFull(); }
        [[nodiscard]] bool IsEmpty() const noexcept { return ringBuffer.IsEmpty(); }

        void SetDebugName(const std::wstring_view debugName) override;

    private:
        RingBuffer ringBuffer;
        void* cpuVirtualAddress;
        D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress;
        ComPtr<ID3D12Resource> gpuBuffer;

    };
}