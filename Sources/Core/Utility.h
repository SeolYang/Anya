#pragma once
#include <PCH.h>

namespace anya
{
    template <size_t BufferSize = 512>
    inline std::wstring AnsiToWString(const char* ansiString)
    {
        std::array<wchar_t, BufferSize> buffer;
        MultiByteToWideChar(CP_ACP, 0, ansiString, -1, buffer.data(), BufferSize);
        return std::wstring(buffer.data());
    }

    template <size_t BufferSize = 512>
    inline std::string WStringToAnsi(const wchar_t* wideString)
    {
        std::array<char, BufferSize> buffer;
        WideCharToMultiByte(CP_ACP, 0, wideString, -1, buffer, BufferSize, NULL, NULL);
        return std::string(buffer.data());
    }
}