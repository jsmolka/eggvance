#pragma once

#include <type_traits>

#include "integer.h"

inline u32 alignHalf(u32 value)
{
    return value & ~0x1;
}

inline u32 alignWord(u32 value)
{
    return value & ~0x3;
}

inline bool misalignedHalf(u32 value)
{
    return value & 0x1;
}

inline bool misalignedWord(u32 value)
{
    return value & 0x3;
}

template<unsigned position, unsigned amount, typename T>
inline int bits(T value)
{
    static_assert(std::is_integral_v<T>, "Expected integral");
    static_assert(position + amount <= 8 * sizeof(T), "Invalid parameters");

    return (value >> position) & ((1 << amount) - 1);
}

#ifdef _MSC_VER
#include <intrin.h>

inline int bitScanForward(u16 value)
{
    unsigned long index = 0;
    if (_BitScanForward(&index, value) == 0)
        return 0;
    return static_cast<int>(index);
}

inline int bitScanReverse(u16 value)
{
    unsigned long index = 0;
    if (_BitScanReverse(&index, value) == 0)
        return 0;
    return static_cast<int>(index);
}

inline int bitCount(u16 value)
{
    return __popcnt16(value);
}

inline u32 rotateRight(u32 value, int amount)
{
    return _rotr(value, amount);
}

#else
#error MSVC specific code
#endif

template<unsigned bits, typename T>
inline T signExtend(T value)
{
    static_assert(std::is_integral_v<T>, "Expected integral");

    using S = typename std::make_signed<T>::type;
    constexpr int amount = 8 * sizeof(T) - bits;
    return static_cast<T>(static_cast<S>(value << amount) >> amount);
}
