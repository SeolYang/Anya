#include <PCH.h>
#include <RHI/PIXMarker.h>
#include <RHI/CommandList.h>

namespace sy::RHI
{
    PIXMarker::PIXMarker(CommandList& cmdList, std::wstring_view message) :
        cmdList(cmdList)
    {
#ifdef USE_PIX
        PIXBeginEvent(cmdList.GetD3DCommandList(), 0, message.data());
#endif
    }

    PIXMarker::PIXMarker(CommandList& cmdList, std::string_view message) :
        cmdList(cmdList)
    {
#ifdef USE_PIX
        PIXBeginEvent(cmdList.GetD3DCommandList(), 0, message.data());
#endif
    }

    PIXMarker::~PIXMarker()
    {
#ifdef USE_PIX
        PIXEndEvent(cmdList.GetD3DCommandList());
#endif
    }
}