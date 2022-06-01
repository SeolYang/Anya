#pragma once
#include <PCH.h>
#include <Core/RingBuffer.h>
#include <Core/Pool.h>
#include <RHI/Descriptor.h>
#include <RHI/DescriptorHeap.h>

// Frame Number = Global Frame Index, Frame Index => Ring Buffer's frame index
namespace sy::RHI
{
    class Device;
    class DescriptorPool
    {
    public:
        template <DescriptorType T>
        using DescriptorPtr = std::unique_ptr<T, std::function<void(T*)>>;
        using CBDescriptorPtr = DescriptorPtr<CBDescriptor>;
        using SRDescriptorPtr = DescriptorPtr<SRDescriptor>;
        using UADescriptorPtr = DescriptorPtr<UADescriptor>;
        using SamplerDescriptorPtr = DescriptorPtr<SamplerDescriptor>;
        using RTDescriptorPtr = DescriptorPtr<RTDescriptor>;
        using DSDescriptorPtr = DescriptorPtr<DSDescriptor>;

        constexpr static size_t MaxNumOfCBDescriptors = 512;
        constexpr static size_t MaxNumOfSRDescriptors = 1024;
        constexpr static size_t MaxNumOfUADescriptors = 1024;
        constexpr static size_t MaxNumOfSamplerDescriptors = 512;
        constexpr static size_t MaxNumOfRTDescriptors = 1024;
        constexpr static size_t MaxNumOfDSDescriptors = MaxNumOfRTDescriptors;

        struct Deallocation
        {
            OffsetPool* Pool;
            OffsetPool::Slot_t Slot;
        };

    public:
        DescriptorPool(Device& device, const size_t simultaneousFramesInFlight);
        ~DescriptorPool() = default;

        void BeginFrame(const uint64 frameNumber);
        void EndFrame(const uint64 frameNumber);

        /** @TODO Implement CB-SR-UA Descriptor Allocations */
        SamplerDescriptorPtr AllocateSamplerDescriptor(const Sampler& sampler);
        DSDescriptorPtr AllocateDepthStencilDescriptor(const Texture& texture);
        RTDescriptorPtr AllocateRenderTargetDescriptor(const Texture& texture, const uint16 mipLevel = 0);

    private:
        void ExecutePendingDeallocations(const uint64 frameIndex);

    private:
        RingBuffer frameIndexTracker;
        size_t currentFrameIndex;

        OffsetPool cbDescriptorPool;
        OffsetPool srDescriptorPool;
        OffsetPool uaDescriptorPool;
        OffsetPool samplerDescriptorPool;
        OffsetPool rtDescriptorPool;
        OffsetPool dsDescriptorPool;

        CBSRUADescriptorHeap cbsruaDescriptorHeap;
        SamplerDescriptorHeap samplerDescriptorHeap;
        RTDescriptorHeap rtDescriptorHeap;
        DSDescriptorHeap dsDescriptorHeap;

        std::vector<std::vector<Deallocation>> pendingDeallocations;

        std::array<CBDescriptor, MaxNumOfCBDescriptors> cbDescriptorHandleCache;
        std::array<SRDescriptor, MaxNumOfSRDescriptors> srDescriptorHandleCache;
        std::array<UADescriptor, MaxNumOfUADescriptors> uaDescriptorHandleCache;
        std::array<SamplerDescriptor, MaxNumOfSamplerDescriptors> samplerDescriptorHandleCache;
        std::array<RTDescriptor, MaxNumOfRTDescriptors> rtDescriptorHandleCache;
        std::array<DSDescriptor, MaxNumOfDSDescriptors> dsDescriptorHandleCache;

    };
}