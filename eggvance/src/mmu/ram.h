#pragma once

#include <array>

#include "base/int.h"

template<typename T, std::size_t N>
class RAM
{
public:
    virtual ~RAM() = default;

    inline std::size_t size() const
    {
        return N;
    }

    template<typename U>
    inline U* data(u32 addr)
    {
        return reinterpret_cast<U*>(&_data[addr]);
    }

    template<typename U>
    inline const U* data(u32 addr) const
    {
        return reinterpret_cast<const U*>(&_data[addr]);
    }

    template<typename U>
    inline U readFast(u32 addr) const
    {
        return *data<U>(addr);
    }

    template<typename U>
    inline void writeFast(u32 addr, U value)
    {
        *data<U>(addr) = value;
    }

    inline u8  readByte(u32 addr) const { return read<u8 >(addr); }
    inline u16 readHalf(u32 addr) const { return read<u16>(addr); }
    inline u32 readWord(u32 addr) const { return read<u32>(addr); }

    inline void writeByte(u32 addr, u8  byte) { write<u8 >(addr, byte); }
    inline void writeHalf(u32 addr, u16 half) { write<u16>(addr, half); }
    inline void writeWord(u32 addr, u32 word) { write<u32>(addr, word); }

protected:
    template<typename U>
    static inline u32 align(u32 addr)
    {
        return addr & ~(sizeof(U) - 1);
    }

private:
    template<typename U>
    inline U read(u32 addr) const
    {
        addr = align<U>(addr);
        addr = T::mirror(addr);

        return readFast<U>(addr);
    }

    template<typename U>
    inline void write(u32 addr, U value)
    {
        addr = align<U>(addr);
        addr = T::mirror(addr);

        writeFast<U>(addr, value);
    }

    std::array<u8, N> _data = { 0 };
};

template<std::size_t N>
class MirroredRAM : public RAM<MirroredRAM<N>, N>
{
public:
    static inline u32 mirror(u32 addr)
    {
        return addr & (N - 1);
    }
};
