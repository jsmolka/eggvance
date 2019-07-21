#pragma once

#include <type_traits>

template<unsigned int position, unsigned int amount, typename T>
inline T bits(T value)
{
    return (value >> position) & ((1 << amount) - 1);
}

template<unsigned int bits, typename T>
inline T signExtend(T value)
{
    using S = typename std::make_signed<T>::type;
    constexpr unsigned int amount = 8 * sizeof(T) - bits;
    return static_cast<T>(static_cast<S>(value << amount) >> amount);
}
