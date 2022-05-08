#pragma once
#include <PCH.h>
#include <Graphics/DXErr.h>

namespace anya
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

    inline void DXCall(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw DXException(hr);
        }
    }

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

    inline void Win32Call(DWORD messageID)
    {
        if (messageID == 0)
        {
            throw Win32Exception(messageID);
        }
    }
}