#include <PCH.h>
#include <RHI/PIXMarker.h>
#include <RHI/CommandList.h>

namespace sy
{
    PIXMarker::PIXMarker(CommandList& cmdList, const std::wstring_view message) :
        cmdList(cmdList)
    {
#ifdef USE_PIX
        PIXBeginEvent(cmdList.D3DCommandList(), 0, message.data());
#endif
    }

    PIXMarker::PIXMarker(CommandList& cmdList, const std::string_view message) :
        cmdList(cmdList)
    {
#ifdef USE_PIX
        PIXBeginEvent(cmdList.D3DCommandList(), 0, message.data());
#endif
    }

    PIXMarker::~PIXMarker()
    {
#ifdef USE_PIX
        PIXEndEvent(cmdList.D3DCommandList());
#endif
    }
}