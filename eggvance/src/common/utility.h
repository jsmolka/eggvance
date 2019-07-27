#pragma once

#include <type_traits>

template<int position, int amount>
inline int bits(int value)
{
    return (value >> position) & ((1 << amount) - 1);
}

template<int bits, typename T>
inline T signExtend(T value)
{
    using S = typename std::make_signed<T>::type;
    constexpr int amount = 8 * sizeof(T) - bits;
    return static_cast<T>(static_cast<S>(value << amount) >> amount);
}
