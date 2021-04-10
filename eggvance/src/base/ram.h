#pragma once

#include <shell/array.h>
#include <shell/macros.h>

#include "base/int.h"

template<uint kSize>
class BasicMirror
{
public:
    u32 operator()(u32 addr) const
    {
        return addr & (kSize - 1);
    }
};

template<uint kSize, typename Mirror = BasicMirror<kSize>>
class Ram : public shell::array<u8, kSize>
{
public:
    template<typename Integral>
    Integral readFast(u32 addr) const
    {
        SHELL_ASSERT(addr + sizeof(Integral) <= kSize);

        return *reinterpret_cast<const Integral*>(this->data() + addr);
    }

    template<typename Integral>
    void writeFast(u32 addr, Integral value)
    {
        SHELL_ASSERT(addr + sizeof(Integral) <= kSize);

        *reinterpret_cast<Integral*>(this->data() + addr) = value;
    }

    u8  readByte(u32 addr) const { return read<u8 >(addr); }
    u16 readHalf(u32 addr) const { return read<u16>(addr); }
    u32 readWord(u32 addr) const { return read<u32>(addr); }

    void writeByte(u32 addr, u8  byte) { write<u8 >(addr, byte); }
    void writeHalf(u32 addr, u16 half) { write<u16>(addr, half); }
    void writeWord(u32 addr, u32 word) { write<u32>(addr, word); }

    const Mirror mirror{};

private:
    template<typename Integral>
    Integral read(u32 addr) const
    {
        addr &= ~(sizeof(Integral) - 1);
        addr = mirror(addr);

        return readFast<Integral>(addr);
    }

    template<typename Integral>
    void write(u32 addr, Integral value)
    {
        addr &= ~(sizeof(Integral) - 1);
        addr = mirror(addr);

        writeFast<Integral>(addr, value);
    }
};
