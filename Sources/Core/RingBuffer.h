#pragma once
#include <PCH.h>

namespace sy
{
    /**
    * @referecne    https://www.codeproject.com/Articles/1094799/Implementing-Dynamic-Resources-with-Direct-D
    */
    class RingBuffer
    {
    public:
        struct FrameTailAttribs
        {
            uint64 FenceValue;
            size_t TailOfFrame;
            size_t SizeOfFrame;
        };

        static constexpr size_t InvalidOffset = std::numeric_limits<size_t>::max();
        using ReleaseCompletedFrameCallback = std::function<void(const FrameTailAttribs&)>;

    public:
        RingBuffer(size_t maxSizeOfBuffer) noexcept;
        RingBuffer(const RingBuffer&) = delete;
        RingBuffer(RingBuffer&& rhs) noexcept;

        RingBuffer& operator=(const RingBuffer&) = delete;
        RingBuffer& operator=(RingBuffer&& rhs) noexcept;

        virtual ~RingBuffer() = default;

        size_t Allocate(const size_t allocSize) noexcept;

        void FinishCurrentFrame(const uint64 currentFrameFenceValue);
        void ReleaseCompletedFrame(const uint64 completedFenceValue);
        void SetReleaseCompletedFrameCallback(const ReleaseCompletedFrameCallback& callback);

        size_t MaxSize() const noexcept { return maxSize; }
        size_t UsedSize() const noexcept { return usedSize; }
        bool IsFull() const noexcept { return usedSize == maxSize; }
        bool IsEmpty() const noexcept { return usedSize == 0; }

    private:
        std::deque<FrameTailAttribs> completedFrameTails;
        ReleaseCompletedFrameCallback releaseCompletedFrameCallback = [](const FrameTailAttribs&) {};
        size_t head = 0;
        size_t tail = 0;
        size_t maxSize = 0;
        size_t usedSize = 0;
        size_t currentFrameSize = 0;

    };
}