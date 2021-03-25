#pragma once

#include <type_traits>
#include <shell/macros.h>

#include "base/bit.h"

template<typename Integral>
class SerialBuffer
{
public:
    static_assert(std::is_integral_v<Integral>);
    static_assert(std::is_unsigned_v<Integral>);

    SerialBuffer& operator=(Integral data)
    {
        this->data = data;
        
        return *this;
    }

    operator Integral() const
    {
        return data;
    }

    u8& operator[](std::size_t index)
    {
        SHELL_ASSERT(index < sizeof(Integral));

        return *(reinterpret_cast<u8*>(&data) + index);
    }

    u8 operator[](std::size_t index) const
    {
        SHELL_ASSERT(index < sizeof(Integral));

        return *(reinterpret_cast<const u8*>(&data) + index);
    }

    void clear()
    {
        data = 0;
        size = 0;
    }

    void pushr(Integral bit)
    {
        SHELL_ASSERT(size < bit::bits_v<Integral>);

        data = (data << 1) | (bit & 0x1);
        size++;
    }

    void pushl(Integral bit)
    {
        SHELL_ASSERT(size < bit::bits_v<Integral>);

        data |= (bit & 0x1) << size;
        size++;
    }

    Integral popr()
    {
        SHELL_ASSERT(size > 0);

        Integral bit = data & 0x1;
        data >>= 1;
        size--;
        return bit;
    }

    Integral popl()
    {
        SHELL_ASSERT(size > 0);

        size--;
        return (data >> size) & 0x1;
    }

    Integral data = 0;
    Integral size = 0;
};
