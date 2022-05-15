#pragma once
#include <PCH.h>
#include <Core/Exceptions.h>

namespace anya::utils
{
    namespace traits_impl
    {
        template <typename T>
        struct is_tuple_impl : std::false_type { };

        template <typename... T>
        struct is_tuple_impl<std::tuple<T...>> : std::true_type { };

        template <typename TupleT, typename ElementT>
        struct is_tuple_element_impl : std::false_type { };

        template <typename ElementT,  typename... ElementTypes>
        struct is_tuple_element_impl<std::tuple<ElementTypes...>, ElementT> :
            std::conditional_t<
                std::disjunction_v<std::is_same<ElementT, ElementTypes>...>,
                std::true_type,
                std::false_type>
        {
        };
    }

    template <typename T>
    using is_tuple = traits_impl::is_tuple_impl<std::decay_t<T>>;

    template <typename T>
    constexpr bool is_tuple_v = is_tuple<T>::value;

    template <typename TupleT, typename ElementT>
    using is_tuple_element = traits_impl::is_tuple_element_impl<std::decay_t<TupleT>, std::decay_t<ElementT>>;

    template <typename TupleT, typename ElementT>
    constexpr bool is_tuple_element_v = is_tuple_element<TupleT, ElementT>::value;

    /** Effective Modern C++ Items 10 */
    template <typename E>
    constexpr auto ToUnderlyingType(E enumerator) noexcept
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