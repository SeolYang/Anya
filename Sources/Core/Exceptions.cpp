#include <PCH.h>
#include <Core/Exceptions.h>
#include <Core/Utility.h>
#include <Core/EngineModuleMediator.h>

namespace sy
{
    Exception::Exception(const std::string& ansiExceptionMessage) :
        message(utils::AnsiToWString(ansiExceptionMessage))
    {
    }
    void DXCall(HRESULT hr)
    {
        if (FAILED(hr))
        {
            EngineModuleMediator::LoggerModule().error(utils::WStringToAnsi(GetDXErrorMessage(hr)));
            throw DXException(hr);
        }
    }
}