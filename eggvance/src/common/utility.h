#pragma once

#include <type_traits>

#include "integer.h"
#include "macros.h"

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

// Todo: remove
template<unsigned position, typename T>
inline bool isset(T value)
{
    static_assert(std::is_integral_v<T>, "Expected integral");
    static_assert(position < 8 * sizeof(T), "Invalid parameter");

    return value & (1 << position);
}

// Todo: check if properly optimized in release
#ifdef _MSC_VER
#include <intrin.h>

template<typename T>
inline int bitScanForward(T value)
{
    static_assert(std::is_integral_v<T>, "Expected integral");

    unsigned long index = 0;
    if (sizeof(T) <= 4)
    {
        _BitScanForward(&index, value);
        return static_cast<int>(index);
    }
    if (sizeof(T) <= 8)
    {
        _BitScanForward64(&index, value);
        return static_cast<int>(index);
    }

    static_assert("Unreachable");
    EGG_UNREACHABLE;
    return 0;
}

template<typename T>
inline int bitScanReverse(T value)
{
    static_assert(std::is_integral_v<T>, "Expected integral");

    unsigned long index = 0;
    if (sizeof(T) <= 4)
    {
        _BitScanReverse(&index, value);
        return static_cast<int>(index);
    }
    if (sizeof(T) <= 8)
    {
        _BitScanReverse64(&index, value);
        return static_cast<int>(index);
    }

    static_assert("Unreachable");
    EGG_UNREACHABLE;
    return 0;
}

template<typename T>
inline int bitCount(T value)
{
    if (sizeof(T) <= 2) return static_cast<int>(__popcnt16(value));
    if (sizeof(T) <= 4) return static_cast<int>(__popcnt(value));
    if (sizeof(T) <= 8) return static_cast<int>(__popcnt64(value));

    static_assert("Unreachable");
    EGG_UNREACHABLE;
    return 0;
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

template<typename T>
inline u8* byteArray(T& data)
{
    return reinterpret_cast<u8*>(&data);
}
