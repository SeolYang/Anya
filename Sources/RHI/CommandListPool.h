#pragma once
#include <PCH.h>
#include <Core/RingBuffer.h>
#include <Core/Pool.h>
#include <RHI/CommandList.h>
#include <RHI/CommandAllocator.h>

#include "Core/TaskPool.h"

namespace sy
{
    class TaskPool;
}

namespace sy::RHI
{
    class Device;
    class CommandListPool
    {
    private:
        template <CommandListType LT, CommandAllocatorType AT>
        struct CommandListPackage
        {
            CommandListPackage(Device& device) :
                CmdAllocator(std::make_unique<AT>(device)),
                CmdListPool(1, 1)
            {
            }

            std::unique_ptr<AT> CmdAllocator;
            std::vector<std::unique_ptr<LT>> CmdLists;
            OffsetPool CmdListPool;
        };

        using CopyCmdListPackage = CommandListPackage<CopyCommandList, CopyCommandAllocator>;
        using ComputeCmdListPackage = CommandListPackage<ComputeCommandList, ComputeCommandAllocator>;
        using DirectCmdListPackage = CommandListPackage < DirectCommandList, DirectCommandAllocator>;

        enum class ECmdListPackageType
        {
            Copy,
            Compute,
            Direct
        };

        template <CommandListType T>
        constexpr static ECmdListPackageType QueryCommandListPackageType();

        template <>
        constexpr static ECmdListPackageType QueryCommandListPackageType<CopyCommandList>()
        {
            return ECmdListPackageType::Copy;
        }

        template <>
        constexpr static ECmdListPackageType QueryCommandListPackageType<ComputeCommandList>()
        {
            return ECmdListPackageType::Compute;
        }

        template <>
        constexpr static ECmdListPackageType QueryCommandListPackageType<DirectCommandList>()
        {
            return ECmdListPackageType::Direct;
        }

        /** Per thread */
        struct PerThreadData
        {
            PerThreadData(Device& device, size_t simultaneousFramesInFlight)
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

            template<CommandListType T>
            auto& QueryCmdListPackage(size_t frameIndex);

            template<>
            auto& QueryCmdListPackage<CopyCommandList>(size_t frameIndex)
            {
                return CopyCmdListPackages[frameIndex];
            }

            template<>
            auto& QueryCmdListPackage<ComputeCommandList>(size_t frameIndex)
            {
                return ComputeCmdListPackages[frameIndex];
            }

            template<>
            auto& QueryCmdListPackage<DirectCommandList>(size_t frameIndex)
            {
                return DirectCmdListPackages[frameIndex];
            }
        };

        struct Deallocation
        {
            OffsetPool* Pool;
            OffsetPool::Slot_t Slot;
        };

    public:
        CommandListPool(Device& device, const TaskPool& taskPool, size_t simultaneousFramesInFlight);

        void BeginFrame(size_t frameNumber);
        void EndFrame(size_t frameNumber);

        template <CommandListType T>
        std::unique_ptr<T, std::function<void(const T*)>> Allocate()
        {
            const size_t threadIndex = TaskPool::ThreadIndex();
            auto& threadData = exclusiveThreadData.at(threadIndex);
            auto& cmdListPackage = threadData->QueryCmdListPackage<T>(currentFrameIndex);

            const OffsetPool::Slot_t allocSlot = cmdListPackage.CmdListPool.Allocate();
            if (allocSlot.Offset >= cmdListPackage.CmdLists.size())
            {
                cmdListPackage.CmdLists.emplace_back(std::make_unique<T>(device, *cmdListPackage.CmdAllocator));
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
        Device& device;
        RingBuffer frameIndexTracker;
        size_t currentFrameIndex;

        std::vector<std::unique_ptr<PerThreadData>> exclusiveThreadData;
        std::vector<std::vector<Deallocation>> pendingDeallocations;

    };
}