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
        DynamicUploadHeap(const Device& device, size_t simultaneousFramesInFlight, size_t initSize, bool bIsCPUAccessible);
        ~DynamicUploadHeap() = default;

        DynamicUploadHeap(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap(DynamicUploadHeap&&) = delete;
        DynamicUploadHeap& operator=(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap& operator=(DynamicUploadHeap&&) = delete;

        /**
        * How to binding allocated resource : https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-descriptors-directly-in-the-root-signature?redirectedfrom=MSDN
        */
        GPURingBuffer::DynamicAllocation Allocate(const size_t sizeInBytes, const size_t resourceAlignment = GPU_DEFAULT_RESOURCE_ALIGNMENT);
        void BeginFrame(uint64 frameNumber);
        void EndFrame(uint64 frameNumber);

    private:
        void CleanUpRedundantBuffers(size_t frameIndex);

    private:
        const Device& device;
        const bool bIsCPUAccessible;
        std::vector<std::vector<GPURingBuffer>> pendingGPURingBuffers;

        RingBuffer frameIndexTracker;
        size_t currentFrameIndex;

    };
}