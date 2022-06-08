#pragma once
#include <PCH.h>
#include <Core/RingBuffer.h>
#include <Core/Pool.h>
#include <RHI/Descriptor.h>
#include <RHI/DescriptorHeap.h>

// Frame Number = Global Frame Index, Frame Index => Ring Buffer's frame index
namespace sy
{
    class RHI::Device;
    class DescriptorPool
    {
    public:
        template <RHI::DescriptorType T>
        struct Allocation
        {
            T Descriptor;
            uint32 Index = 0xffffffff;
        };

        template <RHI::DescriptorType T>
        using AllocationPtr = std::unique_ptr<Allocation<T>, std::function<void(const Allocation<T>*)>>;
        using CBDescAllocPtr = AllocationPtr<RHI::CBDescriptor>;
        using SRDescAllocPtr = AllocationPtr<RHI::SRDescriptor>;
        using UADescAllocPtr = AllocationPtr<RHI::UADescriptor>;
        using SamplerDescAllocPtr = AllocationPtr<RHI::SamplerDescriptor>;
        using RTDescAllocPtr = AllocationPtr<RHI::RTDescriptor>;
        using DSDescAllocPtr = AllocationPtr<RHI::DSDescriptor>;

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
        DescriptorPool(RHI::Device& device, const size_t simultaneousFramesInFlight);
        ~DescriptorPool() = default;

        void BeginFrame(const uint64 frameNumber);
        void EndFrame(const uint64 frameNumber);

        /** @TODO Implement CB-SR-UA Descriptor Allocations */
        [[nodiscard]] SamplerDescAllocPtr AllocateSamplerDescriptor(const RHI::SamplerBuilder& sampler);
        [[nodiscard]] DSDescAllocPtr AllocateDepthStencilDescriptor(const RHI::Texture& texture);
        [[nodiscard]] RTDescAllocPtr AllocateRenderTargetDescriptor(const RHI::Texture& texture, const uint16 mipLevel = 0);

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

        RHI::CBSRUADescriptorHeap cbsruaDescriptorHeap;
        RHI::SamplerDescriptorHeap samplerDescriptorHeap;
        RHI::RTDescriptorHeap rtDescriptorHeap;
        RHI::DSDescriptorHeap dsDescriptorHeap;

        std::vector<std::vector<Deallocation>> pendingDeallocations;

        std::array<Allocation<RHI::CBDescriptor>, MaxNumOfCBDescriptors> cbDescAllocCache;
        std::array<Allocation<RHI::SRDescriptor>, MaxNumOfSRDescriptors> srDescAllocCache;
        std::array<Allocation<RHI::UADescriptor>, MaxNumOfUADescriptors> uaDescAllocCache;
        std::array<Allocation<RHI::SamplerDescriptor>, MaxNumOfSamplerDescriptors> samplerDescAllocCache;
        std::array<Allocation<RHI::RTDescriptor>, MaxNumOfRTDescriptors> rtDescAllocCache;
        std::array<Allocation<RHI::DSDescriptor>, MaxNumOfDSDescriptors> dsDescAllocCache;

    };
}