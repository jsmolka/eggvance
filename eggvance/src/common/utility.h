#pragma once

// Todo: proper template parameter types

#include <type_traits>

#include "integer.h"
#include "tmp.h"

inline u32 align(u32 addr, u32 length)
{
    return addr & ~(length - 1);
}

inline u32 alignHalf(u32 addr)
{
    return addr & ~0x1;
}

inline u32 alignWord(u32 addr)
{
    return addr & ~0x3;
}

inline bool misalignedHalf(u32 addr)
{
    return addr & 0x1;
}

inline bool misalignedWord(u32 addr)
{
    return addr & 0x3;
}

template<int position, int amount, typename T>
inline int bits(T value)
{
    return (value >> position) & ((1 << amount) - 1);
}

template<int position, typename T>
inline bool isset(T value)
{
    return value & (1 << position);
}

template<int bits, typename T>
inline T signExtend(T value)
{
    using S = typename std::make_signed<T>::type;
    constexpr int amount = 8 * sizeof(T) - bits;
    return static_cast<T>(static_cast<S>(value << amount) >> amount);
}

template<typename T>
inline u8* bytes(T* data)
{
    return reinterpret_cast<u8*>(data);
}
