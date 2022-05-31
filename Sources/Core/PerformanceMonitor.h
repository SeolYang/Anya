#pragma once
#include <PCH.h>

namespace sy
{
    class PerformanceMonitor
    {
    public:
        PerformanceMonitor() noexcept = default;
        ~PerformanceMonitor() noexcept = default;

        void UpdateAs(const std::wstring_view key, const uint64 nanos)
        {
            ReadWriteLock lock{mutex};
            monitor[key.data()] = nanos;
        }

        uint64 QueryNanos(const std::wstring_view key) const
        {
            ReadOnlyLock lock{ mutex };
            auto found = monitor.find(key.data());
            if (found != monitor.end())
            {
                return found->second;
            }

            return 0Ui64;
        }

        uint64 QueryMillis(const std::wstring_view key) const
        {
            return QueryNanos(key) / 1000000Ui64;
        }

        float64 QueryDouble(const std::wstring_view key) const
        {
            return QueryNanos(key) * 1e-09;
        }

        float32 Query(const std::wstring_view key) const
        {
            return static_cast<float32>(QueryDouble(key));
        }

    private:
        /** https://kyungpyo-kim.github.io/study/thread-safety-of-unordered_map/ */
        mutable Mutex mutex;
        robin_hood::unordered_map<std::wstring, uint64> monitor;

    };
}