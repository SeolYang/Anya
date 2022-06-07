#include <PCH.h>
#include <Rendering/CommandListPool.h>
#include <RHI/Device.h>
#include <Core/TaskManager.h>

namespace sy
{
    CommandListPool::CommandListPool(RHI::Device& device, const TaskManager& taskManager, size_t simultaneousFramesInFlight) :
        device(device),
        frameIndexTracker(simultaneousFramesInFlight),
        currentFrameIndex(0)
    {
        frameIndexTracker.SetReleaseCompletedFrameCallback(
            [this](const RingBuffer::FrameTailAttribs& frameAttribs)
            {
                const auto frameIndex = frameAttribs.TailOfFrame - frameAttribs.SizeOfFrame;
                ExecutePendingDeallocations(frameIndex);
            });

        const size_t numOfWorkerThreads = taskManager.GetNumWorkerThreads() + 1; // Include Main Thread
        for (auto idx : views::iota(0Ui64, numOfWorkerThreads))
        {
            exclusiveThreadData.emplace_back(std::make_unique<PerThreadData>(device, simultaneousFramesInFlight));
        }

        pendingDeallocations.resize(simultaneousFramesInFlight);
    }

    void CommandListPool::BeginFrame(size_t frameNumber)
    {
        currentFrameIndex = frameIndexTracker.Allocate(1);
        frameIndexTracker.FinishCurrentFrame(frameNumber);
    }

    void CommandListPool::EndFrame(size_t frameNumber)
    {
        frameIndexTracker.ReleaseCompletedFrame(frameNumber);
    }

    void CommandListPool::ExecutePendingDeallocations(size_t frameIndex)
    {
        for (auto& threadData : exclusiveThreadData)
        {
            if (!threadData->CopyCmdListPackages.empty())
            {
                threadData->CopyCmdListPackages.at(frameIndex).CmdAllocator->Reset();
            }

            if (!threadData->ComputeCmdListPackages.empty())
            {
                threadData->ComputeCmdListPackages.at(frameIndex).CmdAllocator->Reset();
            }

            if (!threadData->DirectCmdListPackages.empty())
            {
                threadData->DirectCmdListPackages.at(frameIndex).CmdAllocator->Reset();
            }
        }

        /** Assume it will proceed in main thread. */
        for (Deallocation& deallocation : pendingDeallocations[frameIndex])
        {
            deallocation.Pool->Deallocate(deallocation.Slot);
        }

        pendingDeallocations[frameIndex].clear();
    }
}