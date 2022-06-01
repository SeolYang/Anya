#include <PCH.h>
#include <RHI/DynamicUploadHeap.h>
#include <RHI/Device.h>

namespace sy::RHI
{
    DynamicUploadHeap::DynamicUploadHeap(const Device& device, const size_t initSize, const bool bIsCPUAccessible) :
        device(device),
        bIsCPUAccessible(bIsCPUAccessible)
    {
        gpuRingBuffers.emplace_back(device, initSize, bIsCPUAccessible);
    }

    GPURingBuffer::DynamicAllocation DynamicUploadHeap::Allocate(const size_t sizeInBytes, const size_t resourceAlignment)
    {
        const size_t alignmentMask = resourceAlignment - 1;
        assert((alignmentMask & resourceAlignment) == 0); // Alignment must be power of 2;

        const size_t alignedSize = (sizeInBytes + alignmentMask) & (~alignmentMask);
        auto newAlloc = gpuRingBuffers.back().Allocate(alignedSize);
        if (newAlloc.Buffer == nullptr)
        {
            size_t maxSizeOfNewBuffer = gpuRingBuffers.back().MaxSize() * 2;
            while (maxSizeOfNewBuffer < sizeInBytes)
            {
                maxSizeOfNewBuffer *= 2;
            }

            gpuRingBuffers.emplace_back(device, maxSizeOfNewBuffer, bIsCPUAccessible);
            newAlloc = gpuRingBuffers.back().Allocate(alignedSize);
        }

        return newAlloc;
    }

    void DynamicUploadHeap::FinishFrame(const uint64 fenceValue, const uint64 lastCompletedFenceValue)
    {
        size_t numBuffersToDelete = 0;
        for (size_t idx = 0; idx < gpuRingBuffers.size(); ++idx)
        {
            auto& gpuRingBuffer = gpuRingBuffers[idx];
            gpuRingBuffer.FinishCurrentFrame(fenceValue);
            gpuRingBuffer.ReleaseCompletedFrame(lastCompletedFenceValue);
            if (numBuffersToDelete == idx && idx < gpuRingBuffers.size() - 1 && gpuRingBuffer.IsEmpty())
            {
                ++numBuffersToDelete;
            }
        }

        if (numBuffersToDelete > 0)
        {
            gpuRingBuffers.erase(gpuRingBuffers.begin(), gpuRingBuffers.begin() + numBuffersToDelete);
        }
    }
}