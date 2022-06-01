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
        auto descriptor = samplerDescriptorHeap.Allocate(slot.Offset, sampler);

        return SamplerDescriptorPtr(new decltype(descriptor){ descriptor },
            [this, slot](const decltype(descriptor)* rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &samplerDescriptorPool,
                    .Slot = slot
                    });

                delete rawPtr;
            });
    }

    DescriptorPool::DSDescriptorPtr DescriptorPool::AllocateDepthStencilDescriptor(const Texture& texture)
    {
        auto slot = dsDescriptorPool.Allocate();
        auto descriptor = dsDescriptorHeap.Allocate(slot.Offset, texture);

        return DSDescriptorPtr(new decltype(descriptor){ descriptor },
            [this, slot](const decltype(descriptor)* rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &dsDescriptorPool,
                    .Slot = slot
                    });

                delete rawPtr;
            });
    }

    DescriptorPool::RTDescriptorPtr DescriptorPool::AllocateRenderTargetDescriptor(const Texture& texture, const uint16 mipLevel)
    {
        auto slot = rtDescriptorPool.Allocate();
        auto descriptor = rtDescriptorHeap.Allocate(slot.Offset, texture, mipLevel);

        return RTDescriptorPtr(new decltype(descriptor){ descriptor },
            [this, slot](const decltype(descriptor)* rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &rtDescriptorPool,
                    .Slot = slot
                    });

                delete rawPtr;
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