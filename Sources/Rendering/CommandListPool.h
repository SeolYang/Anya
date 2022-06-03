#pragma once
#include <PCH.h>
#include <Core/RingBuffer.h>
#include <Core/Pool.h>
#include <Core/TaskManager.h>
#include <Core/Utility.h>
#include <RHI/CommandList.h>
#include <RHI/CommandAllocator.h>

namespace sy
{
    namespace RHI
    {
        class Device;
    }

    class CommandListPool
    {
    private:
        template <RHI::CommandListType LT, RHI::CommandAllocatorType AT>
        struct CommandListPackage
        {
            CommandListPackage(RHI::Device& device) :
                CmdAllocator(std::make_unique<AT>(device)),
                CmdListPool(1, 1)
            {
            }

            std::unique_ptr<AT> CmdAllocator;
            std::vector<std::unique_ptr<LT>> CmdLists;
            OffsetPool CmdListPool;
        };

        using CopyCmdListPackage = CommandListPackage<RHI::CopyCommandList, RHI::CopyCommandAllocator>;
        using ComputeCmdListPackage = CommandListPackage<RHI::ComputeCommandList, RHI::ComputeCommandAllocator>;
        using DirectCmdListPackage = CommandListPackage <RHI::DirectCommandList, RHI::DirectCommandAllocator>;

        enum class ECmdListPackageType
        {
            Copy,
            Compute,
            Direct
        };

        template <RHI::CommandListType T>
        constexpr static ECmdListPackageType QueryCommandListPackageType();

        /** Per thread */
        struct PerThreadData
        {
            PerThreadData(RHI::Device& device, size_t simultaneousFramesInFlight)
            {
                for (auto idx : views::iota(0Ui64, simultaneousFramesInFlight))
                {
                    CopyCmdListPackages.emplace_back(device);
                    ComputeCmdListPackages.emplace_back(device);
                    DirectCmdListPackages.emplace_back(device);
                }
            }

            /** CmdListPackage per a frame index */
            std::vector<CopyCmdListPackage> CopyCmdListPackages;
            std::vector<ComputeCmdListPackage> ComputeCmdListPackages;
            std::vector<DirectCmdListPackage> DirectCmdListPackages;

            template<RHI::CommandListType T>
            auto& QueryCmdListPackage(size_t frameIndex);

        };

        struct Deallocation
        {
            OffsetPool* Pool;
            OffsetPool::Slot_t Slot;
        };

    public:
        CommandListPool(RHI::Device& device, const TaskManager& taskManager, size_t simultaneousFramesInFlight);

        void BeginFrame(size_t frameNumber);
        void EndFrame(size_t frameNumber);

        template <RHI::CommandListType T>
        std::unique_ptr<T, std::function<void(const T*)>> Allocate()
        {
            const size_t threadIndex = TaskManager::ThreadIndex();
            auto& threadData = exclusiveThreadData.at(threadIndex);
            auto& cmdListPackage = threadData->QueryCmdListPackage<T>(currentFrameIndex);

            const OffsetPool::Slot_t allocSlot = cmdListPackage.CmdListPool.Allocate();
            if (allocSlot.Offset >= cmdListPackage.CmdLists.size())
            {
                cmdListPackage.CmdLists.emplace_back(std::make_unique<T>(device, *cmdListPackage.CmdAllocator));
                cmdListPackage.CmdLists.back()->SetDebugName(std::format(TEXT("CommandList(Pool) Thread Idx: {} Package: {} Cmd List Offset: {}"), threadIndex, utils::ToUnderlyingType(QueryCommandListPackageType<T>()), allocSlot.Offset));
            }

            T* cmdList = cmdListPackage.CmdLists.at(allocSlot.Offset).get();
            return {cmdList, [this, &cmdListPackage, allocSlot](const T* ptr)
            {
                const Deallocation deallocation{
                    .Pool = &cmdListPackage.CmdListPool,
                    .Slot = allocSlot
                };

                ReadWriteLock lock{ mutex };
                pendingDeallocations[currentFrameIndex].emplace_back(deallocation);
            }};
        }

    private:
        void ExecutePendingDeallocations(size_t frameIndex);

    private:
        Mutex mutex;
        RHI::Device& device;
        RingBuffer frameIndexTracker;
        size_t currentFrameIndex;

        std::vector<std::unique_ptr<PerThreadData>> exclusiveThreadData;
        std::vector<std::vector<Deallocation>> pendingDeallocations;

    };

    template <>
    constexpr CommandListPool::ECmdListPackageType CommandListPool::QueryCommandListPackageType<RHI::CopyCommandList>()
    {
        return ECmdListPackageType::Copy;
    }

    template <>
    constexpr CommandListPool::ECmdListPackageType CommandListPool::QueryCommandListPackageType<RHI::ComputeCommandList>()
    {
        return ECmdListPackageType::Compute;
    }

    template <>
    constexpr CommandListPool::ECmdListPackageType CommandListPool::QueryCommandListPackageType<RHI::DirectCommandList>()
    {
        return ECmdListPackageType::Direct;
    }

    template<>
    inline auto& CommandListPool::PerThreadData::QueryCmdListPackage<RHI::CopyCommandList>(size_t frameIndex)
    {
        return CopyCmdListPackages[frameIndex];
    }

    template<>
    inline auto& CommandListPool::PerThreadData::QueryCmdListPackage<RHI::ComputeCommandList>(size_t frameIndex)
    {
        return ComputeCmdListPackages[frameIndex];
    }

    template<>
    inline auto& CommandListPool::PerThreadData::QueryCmdListPackage<RHI::DirectCommandList>(size_t frameIndex)
    {
        return DirectCmdListPackages[frameIndex];
    }
}