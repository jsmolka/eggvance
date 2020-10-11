#pragma once

#include <array>
#include <type_traits>

#include "base/int.h"

template<uint N>
struct Mirror
{
    u32 operator()(u32 addr) const
    {
        return addr & (N - 1);
    }
};

template<uint N, typename Mirror = Mirror<N>>
class Ram : private std::array<u8, N>
{
public:
    using typename std::array<u8, N>::value_type;
    using std::array<u8, N>::size;
    using std::array<u8, N>::data;
    using std::array<u8, N>::begin;
    using std::array<u8, N>::end;

    Ram()
    {
        this->fill(0);
    }

    template<typename Integral>
    Integral* data(u32 addr)
    {
        static_assert(std::is_integral_v<Integral>);

        return reinterpret_cast<Integral*>(data() + addr);
    }

    template<typename Integral>
    const Integral* data(u32 addr) const
    {
        static_assert(std::is_integral_v<Integral>);

        return reinterpret_cast<const Integral*>(data() + addr);
    }

    template<typename Integral>
    Integral readFast(u32 addr) const
    {
        static_assert(std::is_integral_v<Integral>);

        return *data<Integral>(addr);
    }

    template<typename Integral>
    void writeFast(u32 addr, Integral value)
    {
        static_assert(std::is_integral_v<Integral>);

        *data<Integral>(addr) = value;
    }

    u8  readByte(u32 addr) const { return read<u8 >(addr); }
    u16 readHalf(u32 addr) const { return read<u16>(addr); }
    u32 readWord(u32 addr) const { return read<u32>(addr); }

    void writeByte(u32 addr, u8  byte) { write<u8 >(addr, byte); }
    void writeHalf(u32 addr, u16 half) { write<u16>(addr, half); }
    void writeWord(u32 addr, u32 word) { write<u32>(addr, word); }

    Mirror mirror;

protected:
    template<typename Integral>
    static u32 align(u32 addr)
    {
        static_assert(std::is_integral_v<Integral>);

        return addr & ~(sizeof(Integral) - 1);
    }

private:
    template<typename Integral>
    Integral read(u32 addr) const
    {
        static_assert(std::is_integral_v<Integral>);

        addr = align<Integral>(addr);
        addr = mirror(addr);

        return readFast<Integral>(addr);
    }

    template<typename Integral>
    void write(u32 addr, Integral value)
    {
        static_assert(std::is_integral_v<Integral>);

        addr = align<Integral>(addr);
        addr = mirror(addr);

        writeFast<Integral>(addr, value);
    }
};
