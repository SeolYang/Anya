#pragma once
#include <PCH.h>

namespace sy
{
    class TaskManager;
    class ComponentArchive;
    class Timer;
    class PerformanceMonitor;
    /** 
    * @brief Engine Core Mediator
    */
    class EngineCore
    {
    public:
        EngineCore(TaskManager& taskManager, const Timer& mainTimer, PerformanceMonitor& perfMonitor, Logger& logger, ComponentArchive& componentArchive) noexcept(false) :
            taskManager(taskManager),
            mainTimer(mainTimer),
            perfMonitor(perfMonitor),
            logger(logger),
            componentArchive(componentArchive)
        {
            assert(instance == nullptr);
            std::call_once(instanceFlag, [this]() {
                instance = this;
                });
        }

        ~EngineCore() noexcept
        {
            EngineCore::instance = nullptr;
        }

        static EngineCore& Instance() { assert(instance != nullptr); return *instance; }
        static TaskManager& EngineTaskManager() { return Instance().taskManager; }
        static PerformanceMonitor& EnginePerfMonitor() { return Instance().perfMonitor; }
        static const Timer& EngineTimer() { return Instance().mainTimer; }
        static Logger& EngineLogger() { return Instance().logger; }
        static ComponentArchive& EngineComponentArchive() { return Instance().componentArchive; }

    private:
        inline static std::once_flag instanceFlag;
        inline static EngineCore* instance = nullptr;
        TaskManager& taskManager;
        const Timer& mainTimer;
        PerformanceMonitor& perfMonitor;
        Logger& logger;
        ComponentArchive& componentArchive;

    };
}