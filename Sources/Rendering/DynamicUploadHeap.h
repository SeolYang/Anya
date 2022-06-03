#pragma once
#include <PCH.h>
#include <RHI/GPURingBuffer.h>

namespace sy
{
    class RHI::Device;
    class DynamicUploadHeap
    {
    public:
        DynamicUploadHeap(const RHI::Device& device, size_t initSize, bool bIsCPUAccessible);
        ~DynamicUploadHeap() = default;

        DynamicUploadHeap(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap(DynamicUploadHeap&&) = delete;
        DynamicUploadHeap& operator=(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap& operator=(DynamicUploadHeap&&) = delete;

        /**
        * How to binding allocated resource : https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-descriptors-directly-in-the-root-signature?redirectedfrom=MSDN
        */
        RHI::GPURingBuffer::DynamicAllocation Allocate(const size_t sizeInBytes, const size_t resourceAlignment = RHI::GPU_DEFAULT_RESOURCE_ALIGNMENT);
        void BeginFrame(uint64 frameNumber);
        void EndFrame(uint64 frameNumber);

    private:
        const RHI::Device& device;
        const bool bIsCPUAccessible;
        std::vector<RHI::GPURingBuffer> gpuRingBuffers;

    };
}