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

        [[nodiscard]] static EngineCore& Instance() { assert(instance != nullptr); return *instance; }
        [[nodiscard]] static TaskManager& GetTaskManager() { return Instance().taskManager; }
        [[nodiscard]] static PerformanceMonitor& GetPerfMonitor() { return Instance().perfMonitor; }
        [[nodiscard]] static const Timer& GetTimer() { return Instance().mainTimer; }
        [[nodiscard]] static Logger& GetLogger() { return Instance().logger; }
        [[nodiscard]] static ComponentArchive& GetComponentArchive() { return Instance().componentArchive; }

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