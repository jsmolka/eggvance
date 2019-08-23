#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct KeyControl
{
    template<unsigned index>
    inline void write(u8 byte);

    union
    {
        u8  keys_b[2];  // Key mask bytes
        u16 keys;       // Key mask
    };
    int irq;     // IRQ enable
    int logic;   // IRQ logic (0 = or, 1 = and)
};

template<unsigned index>
inline void KeyControl::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        bytes(&keys)[0] = byte;
        break;

    case 1:
        bytes(&keys)[1] = byte & 0x3;
        irq             = bits<6, 1>(byte);
        logic           = bits<7, 1>(byte);
        break;

    default:
        UNREACHABLE;
        break;
    }
}
