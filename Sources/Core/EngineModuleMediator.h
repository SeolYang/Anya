#pragma once
#include <PCH.h>

namespace sy
{
    class ComponentArchive;
    class Timer;
    class PerformanceMonitor;
    /** 
    * @brief Engine Module Mediator
    */
    class EngineModuleMediator
    {
    public:
        EngineModuleMediator(const Timer& mainTimer, PerformanceMonitor& perfMonitor, Logger& logger, ComponentArchive& componentArchive) noexcept(false) :
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

        ~EngineModuleMediator() noexcept
        {
            EngineModuleMediator::instance = nullptr;
        }

        static EngineModuleMediator& Instance() { assert(instance != nullptr); return *instance; }
        static PerformanceMonitor& PerfMonitor() { return Instance().perfMonitor; }
        static const Timer& MainTimer() { return Instance().mainTimer; }
        static Logger& LoggerModule() { return Instance().logger; }
        static ComponentArchive& ComponentArchiveModule() { return Instance().componentArchive; }

    private:
        inline static std::once_flag instanceFlag;
        inline static EngineModuleMediator* instance;
        const Timer& mainTimer;
        PerformanceMonitor& perfMonitor;
        Logger& logger;
        ComponentArchive& componentArchive;

    };
}