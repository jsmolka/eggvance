#pragma once

#include <climits>
#include <type_traits>

namespace detail
{
    template<typename T>
    constexpr unsigned digits()
    {
        return CHAR_BIT * sizeof(T);
    }
}

template<unsigned index, unsigned size, typename T>
constexpr T bits(T value)
{
    static_assert(index + size <= detail::digits<T>());

    return (value >> index) & ((1ull << size) - 1);
}

template<unsigned size, typename T>
constexpr T signExtend(T value)
{
    static_assert(size <= detail::digits<T>());

    constexpr T mask = 1ull << (size - 1);

    return (value ^ mask) - mask;
}

template<typename T>
constexpr T rotateRight(T value, unsigned amount)
{
    amount %= detail::digits<T>();

    if (amount == 0)
        return value;

    auto x = static_cast<std::make_unsigned_t<T>>(value);

    return (x >> amount) | (x << (detail::digits<T>() - amount));
}

constexpr unsigned popcount(unsigned value)
{
    value = value - ((value >> 1) & 0x5555'5555);
    value = (value & 0x3333'3333) + ((value >> 2) & 0x3333'3333);
    return ((value + (value >> 4) & 0x0F0F'0F0F) * 0x0101'0101) >> 24;
}

constexpr unsigned bitScanForward(unsigned value)
{
    return popcount((value ^ (value - 1)) >> 1);
}

template<typename T>
constexpr unsigned bitScanReverse(T value)
{
    if (value == 0)
        return 0;

    auto x = static_cast<std::make_unsigned_t<T>>(value);

    unsigned index = 0;
    while (x >>= 1)
        index++;

    return index;
}
