#pragma once
#include <PCH.h>

namespace sy
{
    class ComponentArchive;
    /** Engine Module Mediator */
    class EngineModuleMediator
    {
    public:
        EngineModuleMediator(Logger& logger, ComponentArchive& componentArchive) noexcept(false) :
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
        static Logger& LoggerModule() { return Instance().logger; }
        static ComponentArchive& ComponentArchiveModule() { return Instance().componentArchive; }

    private:
        inline static std::once_flag instanceFlag;
        inline static EngineModuleMediator* instance;
        Logger& logger;
        ComponentArchive& componentArchive;

    };
}