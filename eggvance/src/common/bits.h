#pragma once

#include <climits>
#include <type_traits>

#ifdef _MSC_VER
#include <intrin.h>
#endif

template<unsigned index, unsigned size, typename T>
inline T bits(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(index + size <= CHAR_BIT * sizeof(T));

    return (value >> index) & ((1ull << size) - 1);
}

template<unsigned size, typename T>
inline T signExtend(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(size <= CHAR_BIT * sizeof(T));

    T mask = 1ull << (size - 1);

    return (value ^ mask) - mask;
}

inline unsigned rotateRight(unsigned value, unsigned amount)
{
    #ifdef _MSC_VER
    return _rotr(value, amount);
    #else
    amount %= 32;
    if (amount == 0) return value;
    return (value >> amount) | (value << (32 - amount));
    #endif
}

inline unsigned popcount(unsigned value)
{
    #ifdef _MSC_VER
    return __popcnt(value);
    #else
    value = value - ((value >> 1) & 0x5555'5555);
    value = (value & 0x3333'3333) + ((value >> 2) & 0x3333'3333);
    return ((value + (value >> 4) & 0x0F0F'0F0F) * 0x0101'0101) >> 24;
    #endif
}

inline unsigned bitScanForward(unsigned value)
{
    #ifdef _MSC_VER
    unsigned long index = 0;
    if (!_BitScanForward(&index, value))
        return 31;
    return static_cast<unsigned>(index);
    #else
    return popcount((value ^ (value - 1)) >> 1);
    #endif
}
