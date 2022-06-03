#include <PCH.h>
#include <Rendering/DynamicUploadHeap.h>
#include <RHI/Device.h>

namespace sy
{
    DynamicUploadHeap::DynamicUploadHeap(const RHI::Device& device, size_t initSize, bool bIsCPUAccessible) :
        device(device),
        bIsCPUAccessible(bIsCPUAccessible)
    {
        gpuRingBuffers.emplace_back(device, initSize, bIsCPUAccessible);
    }

    RHI::GPURingBuffer::DynamicAllocation DynamicUploadHeap::Allocate(const size_t sizeInBytes, const size_t resourceAlignment)
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

    void DynamicUploadHeap::BeginFrame(uint64 frameNumber)
    {
        for (auto& gpuRingBuffer : gpuRingBuffers)
        {
            gpuRingBuffer.FinishCurrentFrame(frameNumber);
        }
    }

    void DynamicUploadHeap::EndFrame(uint64 frameNumber)
    {
        size_t numOfBuffersToDelete = 0;
        for (size_t idx = 0; idx < gpuRingBuffers.size(); ++idx)
        {
            auto& gpuRingBuffer = gpuRingBuffers[idx];
            gpuRingBuffer.ReleaseCompletedFrame(frameNumber);
            if (numOfBuffersToDelete == idx && idx < gpuRingBuffers.size() - 1 && gpuRingBuffer.IsEmpty())
            {
                ++numOfBuffersToDelete;
            }
        }

        if (numOfBuffersToDelete > 0)
        {
            gpuRingBuffers.erase(gpuRingBuffers.begin(), gpuRingBuffers.begin() + numOfBuffersToDelete);
        }
    }
}