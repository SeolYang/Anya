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
        Exception(const std::wstring& exceptionMessage, const size_t line, const std::wstring_view file) :
            message(std::format(TEXT("{}\nsrc: {}\nline: {}"), exceptionMessage, file, line))
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
        DXException(HRESULT hr, const size_t line, const std::wstring_view file) :
            Exception(GetDXErrorMessage(hr), line, file),
            errorCode(hr)
        {
        }

        [[nodiscard]] HRESULT GetErrorCode() const noexcept
        {
            return errorCode;
        }

    private:
        HRESULT errorCode;

    };

    /** @TODO: Select one between Assert and Exception(or includes more information in debug mode) */

    inline void DXCallInternal(HRESULT hr, const size_t line, const std::wstring file)
    {
        if (FAILED(hr))
        {
            throw DXException(hr, line, file);
        }
    }

    class Win32Exception : public Exception
    {
    public:
        Win32Exception(DWORD messageID, const size_t line, const std::wstring_view file) :
            Exception(GetWin32Message(messageID), line, file),
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

    inline void Win32CallInternal(HRESULT result, const size_t line, const std::wstring& file)
    {
        if (FAILED(result))
        {
            throw Win32Exception(GetLastError(), line, file);
        }
    }

#define DXCall(hr) DXCallInternal(hr, __LINE__, TEXT(__FILE__))
#define Win32Call(result) Win32CallInternal(result, __LINE__, TEXT(__FILE__))

}