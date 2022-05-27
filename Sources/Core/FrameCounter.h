#pragma once
#include <PCH.h>

namespace sy
{
    class FrameCounter
    {
    public:
        FrameCounter() = default;
        ~FrameCounter() = default;

        void Update(const uint64 deltaTimeNanos) noexcept
        {
            constexpr uint64 oneSecAsNanos = chrono::duration_cast<chrono::nanoseconds>(chrono::seconds(1)).count();
            ++frameCount;
            elapsedTimeNanos += deltaTimeNanos;
            if (elapsedTimeNanos >= oneSecAsNanos)
            {
                latestFPS = std::exchange(frameCount, 0);
                elapsedTimeNanos = 0;
            }
        }

        uint64 FrameCount() const noexcept { return frameCount; }
        uint64 LatestFPS() const noexcept { return latestFPS; }

    private:
        uint64 latestFPS = 0;
        uint64 frameCount = 0;
        uint64 elapsedTimeNanos = 0;

    };
}