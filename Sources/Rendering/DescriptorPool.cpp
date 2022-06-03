#include <PCH.h>
#include <Rendering/DescriptorPool.h>

namespace sy
{
    DescriptorPool::DescriptorPool(RHI::Device& device, const size_t simultaneousFramesInFlight) :
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
        frameIndexTracker.SetReleaseCompletedFrameCallback(
            [this](const RingBuffer::FrameTailAttribs& frameAttribs)
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

    DescriptorPool::SamplerDescAllocPtr DescriptorPool::AllocateSamplerDescriptor(const RHI::Sampler& sampler)
    {
        const auto slot = samplerDescriptorPool.Allocate();
        samplerDescAllocCache[slot.Offset] =
            Allocation<RHI::SamplerDescriptor>{
                .Descriptor = samplerDescriptorHeap.Allocate(slot.Offset, sampler),
                .Index = static_cast<uint32>(slot.Offset)
        };

        return SamplerDescAllocPtr(&samplerDescAllocCache[slot.Offset],
            [this, slot](const auto* rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &samplerDescriptorPool,
                    .Slot = slot
                    });
            });
    }

    DescriptorPool::DSDescAllocPtr DescriptorPool::AllocateDepthStencilDescriptor(const RHI::Texture& texture)
    {
        const auto slot = dsDescriptorPool.Allocate();
        dsDescAllocCache[slot.Offset] =
            Allocation<RHI::DSDescriptor>{
                .Descriptor = dsDescriptorHeap.Allocate(slot.Offset, texture),
                .Index = static_cast<uint32>(slot.Offset)
        };

        return DSDescAllocPtr(&dsDescAllocCache[slot.Offset],
            [this, slot](const auto* rawPtr)
            {
                pendingDeallocations[currentFrameIndex].emplace_back(Deallocation{
                    .Pool = &dsDescriptorPool,
                    .Slot = slot
                    });
            });
    }

    DescriptorPool::RTDescAllocPtr DescriptorPool::AllocateRenderTargetDescriptor(const RHI::Texture& texture, const uint16 mipLevel)
    {
        const auto slot = rtDescriptorPool.Allocate();
        rtDescAllocCache[slot.Offset] =
            Allocation<RHI::RTDescriptor>{
                .Descriptor = rtDescriptorHeap.Allocate(slot.Offset, texture, mipLevel),
                .Index = static_cast<uint32>(slot.Offset)
        };

        return RTDescAllocPtr(&rtDescAllocCache[slot.Offset],
            [this, slot](const auto* rawPtr)
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