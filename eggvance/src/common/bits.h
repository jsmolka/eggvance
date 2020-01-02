#pragma once

#include <climits>
#include <type_traits>

template<unsigned index, unsigned size, typename T>
constexpr T bits(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(index + size <= CHAR_BIT * sizeof(T));

    return (value >> index) & ((1ull << size) - 1);
}

template<unsigned size, typename T>
constexpr T signExtend(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(size <= CHAR_BIT * sizeof(T));

    constexpr T mask = 1ull << (size - 1);

    return (value ^ mask) - mask;
}

constexpr unsigned rotateRight(unsigned value, unsigned amount)
{
    amount %= 32;
    if (amount == 0) return value;
    return (value >> amount) | (value << (32 - amount));
}

constexpr unsigned popcount(unsigned value)
{
    value = value - ((value >> 1) & 0x5555'5555);
    value = (value & 0x3333'3333) + ((value >> 2) & 0x3333'3333);
    return ((value + (value >> 4) & 0x0F0F'0F0F) * 0x0101'0101) >> 24;
}

constexpr unsigned scanForward(unsigned value)
{
    return popcount((value ^ (value - 1)) >> 1);
}
