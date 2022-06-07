#pragma once
#include <PCH.h>

namespace sy::RHI
{
    class CommandList;
    class PIXMarker
    {
    public:
        PIXMarker(CommandList& cmdList, std::wstring_view message);
        PIXMarker(CommandList& cmdList, std::string_view message);
        ~PIXMarker();

    private:
        CommandList& cmdList;

    };
}
