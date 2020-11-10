#pragma once

#include <type_traits>

#include "base/bit.h"
#include "base/int.h"
#include "base/macros.h"

template<typename Integral>
class SerialBuffer
{
public:
    static_assert(std::is_integral_v<Integral>);
    static_assert(std::is_unsigned_v<Integral>);

    void clear()
    {
        data = 0;
        size = 0;
    }

    void pushr(uint bit)
    {
        SHELL_ASSERT(size < bit::bits_v<Integral>);

        data = (data << 1) | (bit & 0x1);
        size++;
    }

    void pushl(uint bit)
    {
        SHELL_ASSERT(size < bit::bits_v<Integral>);

        data |= (bit & 0x1) << size;
        size++;
    }

    uint popr()
    {
        SHELL_ASSERT(size > 0);

        uint bit = data & 0x1;
        data >>= 1;
        size--;
        return bit;
    }

    uint popl()
    {
        SHELL_ASSERT(size > 0);

        size--;
        return (data >> size) & 0x1;
    }

    Integral data = 0;
    Integral size = 0;
};
