#pragma once
#include <PCH.h>
#include <Core/DXErr.h>

namespace sy
{
    inline std::wstring GetWin32Message(DWORD messageID)
    {
        wchar_t buffer[MAX_PATH];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
            0,
            messageID,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer,
            MAX_PATH,
            nullptr);

        std::wstring message = TEXT("Win32 Message: ");
        message.append(buffer);
        return message;
    }

    template <uint32 BufferSize = 1024>
    inline std::wstring GetDXErrorMessage(HRESULT hr)
    {
        wchar_t buffer[BufferSize];
        DXGetErrorDescriptionW(hr, buffer, BufferSize);

        std::wstring message = TEXT("DirectX Error: ");
        message.append(buffer);
        return message;
    }

    class Exception
    {
    public:
        Exception() = default;
        Exception(const std::wstring& exceptionMessage) :
            message(exceptionMessage)
        {
        }

        Exception(const std::string& ansiExceptionMessage);

        std::wstring_view GetMessage() const noexcept
        {
            return message;
        }

        void ShowErrorMessageBox() const noexcept
        {
            MessageBox(NULL, message.c_str(), TEXT("ERROR!"), MB_OK | MB_ICONERROR);
        }

    private:
        std::wstring message;

    };

    class DXException : public Exception
    {
    public:
        DXException(HRESULT hr) : 
            Exception(GetDXErrorMessage(hr)),
            errorCode(hr)
        {
        }

        HRESULT GetErrorCode() const noexcept
        {
            return errorCode;
        }

    private:
        HRESULT errorCode;

    };

    /** @TODO: Select one beetween Assert and Exception(or includes more information in debug mode) */
    extern void DXCall(HRESULT hr);

    class Win32Exception : public Exception
    {
    public:
        Win32Exception(DWORD messageID) :
            Exception(GetWin32Message(messageID)),
            messageID(messageID)
        {
        }

        DWORD GetMessageID() const noexcept
        {
            return messageID;
        }

    private:
        DWORD messageID;

    };

    extern void Win32Call(BOOL result);

}