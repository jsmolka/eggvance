#pragma once

#include <type_traits>

#include "integer.h"
#include "tmp.h"

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

inline int countBits(u8 byte)
{
    static constexpr auto counts = makeArray<u8, 256>([](auto x) {
        u8 count = 0;
        for (int temp = static_cast<int>(x); temp != 0; temp >>= 1)
            count += temp & 0x1;
        return count;
    });
    return counts[byte];
}

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
