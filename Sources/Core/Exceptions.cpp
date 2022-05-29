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
            std::wcerr << GetDXErrorMessage(hr) << std::endl;
            throw DXException(hr);
        }
    }

    void Win32Call(BOOL result)
    {
        if (result == 0)
        {
            std::wcerr << GetWin32Message(GetLastError()) << std::endl;
            throw Win32Exception(GetLastError());
        }
    }
}