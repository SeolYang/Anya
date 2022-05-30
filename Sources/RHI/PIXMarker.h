#pragma once
#include <PCH.h>

namespace sy::RHI
{
    class CommandList;
    class PIXMarker
    {
    public:
        PIXMarker(CommandList& cmdList, const std::wstring_view message);
        PIXMarker(CommandList& cmdList, const std::string_view message);
        ~PIXMarker();

    private:
        CommandList& cmdList;

    };
}
