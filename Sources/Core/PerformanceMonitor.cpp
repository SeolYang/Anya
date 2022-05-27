#include <PCH.h>
#include <Core/PerformanceMonitor.h>
#include <Core/Timer.h>

namespace sy
{
    void PerformanceMonitor::Register(const std::wstring_view key, const std::reference_wrapper<const Timer> timer)
    {
        monitor[key.data()] = timer;
    }

    Timer PerformanceMonitor::Query(const std::wstring_view key) const
    {
        Timer timer;
        if (monitor.find(key.data()) != monitor.end())
        {
            timer = monitor.at(key.data());
        }

        return timer;
    }
}