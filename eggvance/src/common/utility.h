#pragma once

#include <type_traits>

#include "integer.h"

// Todo: remove
inline u32 align(u32 value, u32 length)
{
    return value & ~(length - 1);
}

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

template<unsigned position, typename T>
inline bool isset(T value)
{
    static_assert(std::is_integral_v<T>, "Expected integral");
    static_assert(position < 8 * sizeof(T), "Invalid parameter");

    return value & (1 << position);
}

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
