#pragma once

#include <array>

#include "base/int.h"

template<uint N>
class BasicMirror
{
public:
    u32 operator()(u32 addr) const
    {
        return addr & (N - 1);
    }
};

template<uint N, typename Mirror = BasicMirror<N>>
class Ram : public std::array<u8, N>
{
public:
    template<typename Integral>
    Integral readFast(u32 addr) const
    {
        return *reinterpret_cast<const Integral*>(data() + addr);
    }

    template<typename Integral>
    void writeFast(u32 addr, Integral value)
    {
        *reinterpret_cast<Integral*>(data() + addr) = value;
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
