#pragma once

#include <cstddef>
#include <cstring>

#include "common/integer.h"

template<std::size_t size_, std::size_t align_ = size_>
class RAM
{
public:
    virtual ~RAM() = default;

    template<typename T>
    inline T* data(std::size_t addr)
    {
        return reinterpret_cast<T*>(&ram[addr]);
    }

    inline std::size_t size() const
    {
        return size_;
    }

    inline void fill(u8 value)
    {
        std::memset(ram, value, sizeof(ram));
    }

    inline u8  readByte(u32 addr) { return read<u8 >(addr); }
    inline u16 readHalf(u32 addr) { return read<u16>(addr); }
    inline u32 readWord(u32 addr) { return read<u32>(addr); }

    inline void writeByte(u32 addr, u8  byte) { return write<u8 >(addr, byte); }
    inline void writeHalf(u32 addr, u16 half) { return write<u16>(addr, half); }
    inline void writeWord(u32 addr, u32 word) { return write<u32>(addr, word); }

    inline u8  readByteFast(u32 addr) { return *data<u8 >(addr); }
    inline u16 readHalfFast(u32 addr) { return *data<u16>(addr); }
    inline u32 readWordFast(u32 addr) { return *data<u32>(addr); }

private:
    template<typename T>
    inline T read(u32 addr)
    {
        addr &= align_ - sizeof(T);
        return *data<T>(mirror(addr));
    }

    template<typename T>
    inline void write(u32 addr, T value)
    {
        addr &= align_ - sizeof(T);
        *data<T>(mirror(addr)) = value;
    }

    inline virtual u32 mirror(u32 addr) const
    {
        return addr;
    }

    u8 ram[size_];
};
