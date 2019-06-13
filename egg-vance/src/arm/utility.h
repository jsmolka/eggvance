#pragma once

#include "common/integer.h"

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

// Todo: replace with signExtend?
template<unsigned int size>
inline s32 twos(u32 value)
{
    static_assert(size <= 32, "Invalid size");

    if (value & (1 << (size - 1)))
        value |= 0xFFFFFFFF << size;

    return static_cast<s32>(value);
}

template<>
inline s32 twos<32>(u32 value)
{
    return static_cast<s32>(value);
}
