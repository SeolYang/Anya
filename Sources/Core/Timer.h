#pragma once
#include <PCH.h>

namespace sy
{
    class Timer
    {
    public:
        Timer() noexcept = default;

        Timer(const Timer&) = default;
        Timer(Timer&&) noexcept = default;

        Timer& operator=(const Timer&) = default;
        Timer& operator=(Timer&&) noexcept = default;

        void Begin() noexcept
        {
            begin = clock.now();
        }

        void End() noexcept
        {
            deltaTime = (clock.now() - begin);
        }

        uint64 DeltaTimeNanos() const noexcept { return deltaTime.count(); }
        uint64 DeltaTimeMillis() const noexcept { return chrono::duration_cast<chrono::milliseconds>(deltaTime).count(); }
        float64 DeltaTimeDouble() const noexcept { return DeltaTimeMillis() / 1000.0; }
        float32 DeltaTime() const noexcept { return DeltaTimeMillis() / 1000.0f; }

    private:
        chrono::high_resolution_clock clock;
        chrono::steady_clock::time_point begin;
        chrono::nanoseconds deltaTime;

    };
}
