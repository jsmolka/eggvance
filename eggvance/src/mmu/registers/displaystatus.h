#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct DisplayStatus
{
    void reset();

    template<unsigned index>
    inline u8 read();

    template<unsigned index>
    inline void write(u8 byte);

    int vblank;          // V-Blank flag (set in lines 160..226, not 227)
    int hblank;          // H-Blank flag (toggled in all lines 0..227)
    int vmatch;          // V-Count match flag (set in selected line)
    int vblank_irq;      // V-Blank IRQ enable
    int hblank_irq;      // H-Blank IRQ enable
    int vmatch_irq;      // V-Count match IRQ enable
    int vcount_compare;  // Compare value for V-Count
};

template<unsigned index>
inline u8 DisplayStatus::read()
{
    static_assert(index <= 1);

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= vblank     << 0;
        byte |= hblank     << 1;
        byte |= vmatch     << 2;
        byte |= vblank_irq << 3;
        byte |= hblank_irq << 4;
        byte |= vmatch_irq << 5;
        break;

    case 1:
        byte = vcount_compare;
        break;

    default:
        UNREACHABLE;
        break;
    }
    return byte;
}

template<unsigned index>
inline void DisplayStatus::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        vblank_irq = bits<3, 1>(byte);
        hblank_irq = bits<4, 1>(byte);
        vmatch_irq = bits<5, 1>(byte);
        break;

    case 1:
        vcount_compare = byte;
        break;

    default:
        UNREACHABLE;
        break;
    }
}
