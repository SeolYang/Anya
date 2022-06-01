#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/GPURingBuffer.h>

namespace sy::RHI
{
    class Device;
    class DynamicUploadHeap
    {
    public:
        DynamicUploadHeap(const Device& device, const size_t initSize, const bool bIsCPUAccessible);
        ~DynamicUploadHeap() = default;

        DynamicUploadHeap(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap(DynamicUploadHeap&&) = delete;
        DynamicUploadHeap& operator=(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap& operator=(DynamicUploadHeap&&) = delete;

        /**
        * How to binding allocated resource : https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-descriptors-directly-in-the-root-signature?redirectedfrom=MSDN
        */
        GPURingBuffer::DynamicAllocation Allocate(const size_t sizeInBytes, const size_t resourceAlignment = GPU_DEFAULT_RESOURCE_ALIGNMENT);
        void FinishFrame(const uint64 fenceValue, const uint64 lastCompletedFenceValue);

    private:
        const Device& device;
        const bool bIsCPUAccessible;
        std::vector<GPURingBuffer> gpuRingBuffers;

    };
}