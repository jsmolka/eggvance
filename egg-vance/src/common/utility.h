#pragma once

#include <type_traits>

template<typename T, unsigned int bits>
T signExtend(T value)
{
    static_assert(std::is_integral<T>::value, "Integral required");

    using S = typename std::make_signed<T>::type;

    constexpr unsigned int amount = 8 * sizeof(T) - bits;

    return static_cast<T>(static_cast<S>(value << amount) >> amount);
}
