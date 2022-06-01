#include <PCH.h>
#include <RHI/DescriptorPool.h>

namespace sy::RHI
{
    DescriptorPool::DescriptorPool(Device& device, const size_t simultaneousFramesInFlight) :
        frameIndexTracker{ simultaneousFramesInFlight },
        currentFrameIndex{ 0 },
        cbDescriptorPool{ 1, MaxNumOfCBDescriptors },
        srDescriptorPool{ 1, MaxNumOfSRDescriptors },
        uaDescriptorPool{ 1, MaxNumOfUADescriptors },
        samplerDescriptorPool{ 1, MaxNumOfSamplerDescriptors },
        rtDescriptorPool{ 1, MaxNumOfRTDescriptors },
        dsDescriptorPool{ 1, MaxNumOfDSDescriptors },
        cbsruaDescriptorHeap{ device, MaxNumOfCBDescriptors, MaxNumOfSRDescriptors, MaxNumOfUADescriptors },
        samplerDescriptorHeap{ device, MaxNumOfSamplerDescriptors },
        rtDescriptorHeap{ device, MaxNumOfRTDescriptors },
        dsDescriptorHeap{ device, MaxNumOfDSDescriptors }
    {
        frameIndexTracker.SetReleaseCompletedFrameCallback([this](const RingBuffer::FrameTailAttribs& frameAttribs)
            {
                const auto frameIndex = frameAttribs.TailOfFrame - frameAttribs.SizeOfFrame;
                ExecutePendingDeallocations(frameIndex);
            });

        pendingDeallocations.resize(simultaneousFramesInFlight);
    }

    void DescriptorPool::BeginFrame(const uint64 frameNumber)
    {
        currentFrameIndex = frameIndexTracker.Allocate(1);
        frameIndexTracker.FinishCurrentFrame(frameNumber);
    }

    void DescriptorPool::EndFrame(const uint64 frameNumber)
    {
        frameIndexTracker.ReleaseCompletedFrame(frameNumber);
    }

    DescriptorPool::SamplerDescriptorPtr DescriptorPool::AllocateSamplerDescriptor(const Sampler& sampler)
    {
        auto slot = samplerDescriptorPool.Allocate();
        samplerDescriptorHandleCache[slot.Offset] = samplerDescriptorHeap.Allocate(slot.Offset, sampler);

        return SamplerDescriptorPtr(&samplerDescriptorHandleCache[slot.Offset],
            [this, slot](const decltype(samplerDescriptorHandleCache.data()) rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &samplerDescriptorPool,
                    .Slot = slot
                    });
            });
    }

    DescriptorPool::DSDescriptorPtr DescriptorPool::AllocateDepthStencilDescriptor(const Texture& texture)
    {
        auto slot = dsDescriptorPool.Allocate();
        dsDescriptorHandleCache[slot.Offset] = dsDescriptorHeap.Allocate(slot.Offset, texture);

        return DSDescriptorPtr(&dsDescriptorHandleCache[slot.Offset],
            [this, slot](const decltype(dsDescriptorHandleCache.data()) rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &dsDescriptorPool,
                    .Slot = slot
                    });
            });
    }

    DescriptorPool::RTDescriptorPtr DescriptorPool::AllocateRenderTargetDescriptor(const Texture& texture, const uint16 mipLevel)
    {
        auto slot = dsDescriptorPool.Allocate();
        rtDescriptorHandleCache[slot.Offset] = rtDescriptorHeap.Allocate(slot.Offset, texture, mipLevel);

        return RTDescriptorPtr(&rtDescriptorHandleCache[slot.Offset],
            [this, slot](const decltype(rtDescriptorHandleCache.data()) rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &rtDescriptorPool,
                    .Slot = slot
                    });
            });
    }

    void DescriptorPool::ExecutePendingDeallocations(const uint64 frameIndex)
    {
        for (Deallocation& dealloc : pendingDeallocations[frameIndex])
        {
            dealloc.Pool->Deallocate(dealloc.Slot);
        }

        pendingDeallocations[frameIndex].clear();
    }
}