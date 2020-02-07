#pragma once

#include <array>

namespace mpl
{
    namespace detail
    {
        template<typename T, typename Func, std::size_t ...Is>
        constexpr std::array<T, sizeof...(Is)> arrayImpl(Func&& func, std::index_sequence<Is...>)
        {
            return { func(std::integral_constant<std::size_t, Is>{})... };
        }
    }

    template<typename T, std::size_t N, typename Func>
    constexpr std::array<T, N> array(Func&& func)
    {
        return detail::arrayImpl<T>(std::forward<Func>(func), std::make_index_sequence<N>{});
    }
}
