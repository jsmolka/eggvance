#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct DMAControl
{
    void reset();

    template<unsigned index>
    inline u8 read();

    template<unsigned index>
    inline void write(u8 byte);

    int sad_control;  // Destination address control
    int dad_control;  // Source address control
    int repeat;       // DMA repeat
    int word;         // DMA transfer type
    int drq;          // ???
    int timing;       // Start timing
    int irq;          // IRQ on end of word count
    int enable;       // DMA enable

    int count;        // Word count
    int count_mask;   // Word count mask

    bool reload;
};

template<unsigned index>
inline u8 DMAControl::read()
{
    static_assert(index == 2 || index == 3);

    u8 byte = 0;
    switch (index)
    {
    case 2:
        byte |= dad_control << 5;
        byte |= sad_control << 7;
        break;

    case 3:
        byte |= sad_control >> 1;
        byte |= repeat      << 1;
        byte |= word        << 2;
        byte |= drq         << 3;
        byte |= timing      << 4;
        byte |= irq         << 6;
        byte |= enable      << 7;
        break;

    default:
        UNREACHABLE;
        break;
    }
    return byte;
}

template<unsigned index>
inline void DMAControl::write(u8 byte)
{
    static_assert(index <= 3);

    switch (index)
    {
    case 0:
    case 1:
        bytes(&count)[index] = byte;
        count &= count_mask;
        if (count == 0)
            count = count_mask + 1;
        break;

    case 2:
        dad_control = bits<5, 2>(byte);
        sad_control = bits<7, 1>(byte) << 0 | (sad_control & ~0x1);
        break;

    case 3:
        sad_control = bits<0, 1>(byte) << 1 | (sad_control & ~0x2);
        repeat     = bits<1, 1>(byte);
        word       = bits<2, 1>(byte);
        drq        = bits<3, 1>(byte);
        timing     = bits<4, 2>(byte);
        irq        = bits<6, 1>(byte);
        enable     = bits<7, 1>(byte);
        reload     = enable;
        break;

    default:
        UNREACHABLE;
        break;
    }
}
