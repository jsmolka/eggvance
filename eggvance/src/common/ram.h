#pragma once

#include <array>

#include "common/integer.h"

template<std::size_t size>
class RAM : public std::array<u8, size>
{
public:
    template<typename T>
    inline T* ptr(std::size_t index)
    {
        return reinterpret_cast<T*>(this->data() + index);
    }

    inline u8 readByte(u32 addr)
    {
        addr &= size - 1;
        return *ptr<u8>(mirror(addr));
    }

    inline u16 readHalf(u32 addr)
    {
        addr &= size - 2;
        return *ptr<u16>(mirror(addr));
    }

    inline u32 readWord(u32 addr)
    {
        addr &= size - 4;
        return *ptr<u32>(mirror(addr));
    }

    inline void writeByte(u32 addr, u8 byte)
    {
        addr &= size - 1;
        *ptr<u8>(mirror(addr)) = byte;
    }

    inline void writeHalf(u32 addr, u16 half)
    {
        addr &= size - 2;
        *ptr<u16>(mirror(addr)) = half;
    }

    inline void writeWord(u32 addr, u32 word)
    {
        addr &= size - 4;
        *ptr<u32>(mirror(addr)) = word;
    }

private:
    inline virtual u32 mirror(u32 addr)
    {
        return addr;
    }
};
