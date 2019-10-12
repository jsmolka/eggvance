#pragma once

#include <array>

#include "common/integer.h"

template<std::size_t N>
struct RAM : public std::array<u8, N>
{
    template<typename T>
    inline T* ptr(std::size_t index)
    {
        return reinterpret_cast<T*>(this->data() + index);
    }

    inline u8 readByte(u32 addr)
    {
        return *ptr<u8>(addr);
    }

    inline u16 readHalf(u32 addr)
    {
        return *ptr<u16>(addr);
    }

    inline u32 readWord(u32 addr)
    {
        return *ptr<u32>(addr);
    }

    inline void writeByte(u32 addr, u8 byte)
    {
        *ptr<u8>(addr) = byte;
    }

    inline void writeHalf(u32 addr, u16 half)
    {
        *ptr<u16>(addr) = half;
    }

    inline void writeWord(u32 addr, u32 word)
    {
        *ptr<u32>(addr) = word;
    }
};
