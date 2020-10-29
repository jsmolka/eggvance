#pragma once

#include <shell/traits.h>

#include "base/int.h"
#include "base/macros.h"

template<uint N>
class Mirror
{
public:
    u32 operator()(u32 addr) const
    {
        return addr & (N - 1);
    }
};

template<uint N, typename Mirror = Mirror<N>>
class Ram
{
public:
    using value_type = u8;

    constexpr uint size() const { return N; }

    u8* data() { return data_; }
    const u8* data() const { return data_; }

    u8* begin() { return data_; }
    u8* end() { return data_ + N; };
    const u8* begin() const { return data_; }
    const u8* end() const { return data_ + N; }
    const u8* cbegin() const { return data_; }
    const u8* cend() const { return data_ + N; }

    template<typename Integral>
    Integral* data(u32 addr)
    {
        static_assert(is_memory_type<Integral>::value);
        SHELL_ASSERT(addr < N);

        return reinterpret_cast<Integral*>(data() + addr);
    }

    template<typename Integral>
    const Integral* data(u32 addr) const
    {
        static_assert(is_memory_type<Integral>::value);
        SHELL_ASSERT(addr < N);

        return reinterpret_cast<const Integral*>(data() + addr);
    }

    template<typename Integral>
    Integral readFast(u32 addr) const
    {
        static_assert(is_memory_type<Integral>::value);

        return *data<Integral>(addr);
    }

    template<typename Integral>
    void writeFast(u32 addr, Integral value)
    {
        static_assert(is_memory_type<Integral>::value);

        *data<Integral>(addr) = value;
    }

    u8  readByte(u32 addr) const { return read<u8 >(addr); }
    u16 readHalf(u32 addr) const { return read<u16>(addr); }
    u32 readWord(u32 addr) const { return read<u32>(addr); }

    void writeByte(u32 addr, u8  byte) { write<u8 >(addr, byte); }
    void writeHalf(u32 addr, u16 half) { write<u16>(addr, half); }
    void writeWord(u32 addr, u32 word) { write<u32>(addr, word); }

    const Mirror mirror = Mirror();

protected:
    template<typename Integral>
    static u32 align(u32 addr)
    {
        static_assert(is_memory_type<Integral>::value);

        return addr & ~(sizeof(Integral) - 1);
    }

private:
    template<typename T>
    using is_memory_type = shell::is_any_of<T, u8, u16, u32>;

    template<typename Integral>
    Integral read(u32 addr) const
    {
        static_assert(is_memory_type<Integral>::value);

        addr = align<Integral>(addr);
        addr = mirror(addr);

        return readFast<Integral>(addr);
    }

    template<typename Integral>
    void write(u32 addr, Integral value)
    {
        static_assert(is_memory_type<Integral>::value);

        addr = align<Integral>(addr);
        addr = mirror(addr);

        writeFast<Integral>(addr, value);
    }

    u8 data_[N] = {};
};
