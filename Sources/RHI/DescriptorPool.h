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
        struct Allocation
        {
            T Descriptor;
            uint32 Index = 0xffffffff;
        };

        template <DescriptorType T>
        using AllocationPtr = std::unique_ptr<Allocation<T>, std::function<void(const Allocation<T>*)>>;
        using CBDescAllocPtr = AllocationPtr<CBDescriptor>;
        using SRDescAllocPtr = AllocationPtr<SRDescriptor>;
        using UADescAllocPtr = AllocationPtr<UADescriptor>;
        using SamplerDescAllocPtr = AllocationPtr<SamplerDescriptor>;
        using RTDescAllocPtr = AllocationPtr<RTDescriptor>;
        using DSDescAllocPtr = AllocationPtr<DSDescriptor>;

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
        SamplerDescAllocPtr AllocateSamplerDescriptor(const Sampler& sampler);
        DSDescAllocPtr AllocateDepthStencilDescriptor(const Texture& texture);
        RTDescAllocPtr AllocateRenderTargetDescriptor(const Texture& texture, const uint16 mipLevel = 0);

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

        std::array<Allocation<CBDescriptor>, MaxNumOfCBDescriptors> cbDescAllocCache;
        std::array<Allocation<SRDescriptor>, MaxNumOfSRDescriptors> srDescAllocCache;
        std::array<Allocation<UADescriptor>, MaxNumOfUADescriptors> uaDescAllocCache;
        std::array<Allocation<SamplerDescriptor>, MaxNumOfSamplerDescriptors> samplerDescAllocCache;
        std::array<Allocation<RTDescriptor>, MaxNumOfRTDescriptors> rtDescAllocCache;
        std::array<Allocation<DSDescriptor>, MaxNumOfDSDescriptors> dsDescAllocCache;

    };
}