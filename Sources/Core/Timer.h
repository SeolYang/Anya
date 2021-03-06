#pragma once
#include <PCH.h>

namespace sy
{
    class Timer
    {
    public:
        Timer() noexcept = default;
        ~Timer() = default;

        Timer(const Timer&) = default;
        Timer(Timer&&) noexcept = default;

        Timer& operator=(const Timer&) = default;
        Timer& operator=(Timer&&) noexcept = default;

        void Begin() noexcept
        {
            begin = chrono::high_resolution_clock::now();
        }

        void End() noexcept
        {
            deltaTime = (chrono::high_resolution_clock::now() - begin);
        }

        [[nodiscard]] uint64 GetDeltaTimeNanos() const noexcept { return deltaTime.count(); }
        [[nodiscard]] uint64 GetDeltaTimeMillis() const noexcept { return chrono::duration_cast<chrono::milliseconds>(deltaTime).count(); }
        [[nodiscard]] float64 GetDeltaTimeDouble() const noexcept { return GetDeltaTimeNanos() * 1e-09; }
        [[nodiscard]] float32 GetDeltaTime() const noexcept { return static_cast<float>(GetDeltaTimeDouble()); }

    private:
        chrono::steady_clock::time_point begin = chrono::high_resolution_clock::now();
        chrono::nanoseconds deltaTime = chrono::nanoseconds(0);

    };
}
