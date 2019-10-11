#pragma once

#include <array>

namespace detail
{
    template<typename T, class Func, std::size_t ...Is>
    constexpr std::array<T, sizeof...(Is)> makeArrayImpl(Func&& func, std::index_sequence<Is...>)
    {
        return { func(std::integral_constant<std::size_t, Is>{})... };
    }
}

template<typename T, std::size_t N, class Func>
constexpr std::array<T, N> makeArray(Func&& func)
{
    return detail::makeArrayImpl<T>(std::forward<Func>(func), std::make_index_sequence<N>{});
}
