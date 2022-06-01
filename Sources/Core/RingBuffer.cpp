#include <PCH.h>
#include <Core/RingBuffer.h>

namespace sy
{
    RingBuffer::RingBuffer(size_t maxSizeOfBuffer) noexcept :
        head(0),
        tail(0),
        maxSize(maxSizeOfBuffer),
        usedSize(0),
        currentFrameSize(0)
    {
    }

    RingBuffer::RingBuffer(RingBuffer&& rhs) noexcept :
        completedFrameTails(std::move(rhs.completedFrameTails)),
        head(std::exchange(rhs.head, 0)),
        tail(std::exchange(rhs.tail, 0)),
        maxSize(std::exchange(rhs.maxSize, 0)),
        usedSize(std::exchange(rhs.usedSize, 0)),
        currentFrameSize(std::exchange(rhs.currentFrameSize, 0))
    {
    }

    RingBuffer& RingBuffer::operator=(RingBuffer&& rhs) noexcept
    {
        completedFrameTails = std::move(rhs.completedFrameTails);
        head = std::exchange(rhs.head, 0);
        tail = std::exchange(rhs.tail, 0);
        maxSize = std::exchange(rhs.maxSize, 0);
        usedSize = std::exchange(rhs.usedSize, 0);
        currentFrameSize = std::exchange(rhs.currentFrameSize, 0);

        return (*this);
    }

    size_t RingBuffer::Allocate(const size_t allocSize) noexcept
    {
        size_t offset = RingBuffer::InvalidOffset;
        if (!IsFull())
        {
            if (tail >= head)
            {
                //                     Head             Tail     MaxSize
                //                     |                |        |
                //  [                  xxxxxxxxxxxxxxxxx         ]
                /** Enough space for new allocation at behind of tail. */
                if (tail + allocSize <= maxSize)
                {
                    offset = tail;
                    tail += allocSize;
                    usedSize += allocSize;
                    currentFrameSize += allocSize;
                }
                /** Enough space for new allocation at infront of head. */
                else if (allocSize <= head)
                {
                    offset = 0;
                    size_t actualAllocSize = (maxSize - tail) + allocSize;
                    usedSize += actualAllocSize;
                    currentFrameSize += actualAllocSize;
                    tail = allocSize;
                }
            }
            /** Enough space for new allocation at between tail and head. */
            else if (tail + allocSize <= head)
            {
                //       Tail          Head            
                //       |             |            
                //  [xxxx              xxxxxxxxxxxxxxxxxxxxxxxxxx]
                offset = tail;
                tail += allocSize;
                usedSize += allocSize;
                currentFrameSize += allocSize;
            }
        }

        return offset;
    }

    void RingBuffer::FinishCurrentFrame(const uint64 currentFrameFenceValue)
    {
        completedFrameTails.emplace_back(FrameTailAttribs{
                .FenceValue = currentFrameFenceValue,
                .TailOfFrame = tail,
                .SizeOfFrame = currentFrameSize
            });
        currentFrameSize = 0;
    }

    void RingBuffer::ReleaseCompletedFrame(const uint64 completedFenceValue)
    {
        while (!completedFrameTails.empty() &&
            completedFrameTails.front().FenceValue <= completedFenceValue)
        {
            const auto& oldestFrameTail = completedFrameTails.front();
            assert(oldestFrameTail.SizeOfFrame <= usedSize);
            usedSize -= oldestFrameTail.SizeOfFrame;
            head = oldestFrameTail.TailOfFrame;
            releaseCompletedFrameCallback(oldestFrameTail);
            completedFrameTails.pop_front();
        }
    }

    void RingBuffer::SetReleaseCompletedFrameCallback(const ReleaseCompletedFrameCallback& callback)
    {
        releaseCompletedFrameCallback = callback;
    }
}