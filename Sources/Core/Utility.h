#pragma once
#include <PCH.h>
#include <Core/Exceptions.h>

namespace anya
{
    /** Effective Modern C++ Items 10 */
    template <typename E>
    constexpr auto ToUnderlying(E enumerator) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(enumerator);
    }

    template <size_t BufferSize = 512>
    inline std::wstring AnsiToWString(std::string_view ansiString)
    {
        std::array<wchar_t, BufferSize> buffer;
        Win32Call(MultiByteToWideChar(CP_ACP, 0, ansiString.data(), -1, buffer.data(), BufferSize));
        return std::wstring(buffer.data());
    }

    template <size_t BufferSize = 512>
    inline std::string WStringToAnsi(std::wstring_view wideString)
    {
        std::array<char, BufferSize> buffer;
        Win32Call(WideCharToMultiByte(CP_ACP, 0, wideString.data(), -1, buffer.data(), BufferSize, NULL, NULL));
        return std::string(buffer.data());
    }
}