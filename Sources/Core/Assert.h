#pragma once
#include <PCH.h>
#include <Core/EngineCoreMediator.h>

namespace sy
{
    static void AssertInternal(bool bCondition, std::string_view file, size_t line, std::string_view message)
    {
#if defined(_DEBUG) || defined(DEBUG)
        if (!bCondition)
        {
            auto& logger = EngineCore::EngineLogger();
            logger.critical(std::format("Assert failed at File : {}, Line : {}, Message : {}", file, line, message));
            __debugbreak();
        }
#endif
    }
}

#define ANYA_ASSERT(Condition, LogMessage) sy::AssertInternal(Condition, __FILE__, __LINE__, LogMessage);