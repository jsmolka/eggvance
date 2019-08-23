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

    int count;       // Word count
    int count_mask;  // Word count mask

    int src_adjust;  // Destination address control
    int dst_adjust;  // Source address control
    int repeat;      // DMA repeat
    int word;        // DMA transfer type
    int drq;         // ???
    int timing;      // Start timing
    int irq;         // IRQ on end of word count
    int enable;      // DMA enable
};

template<unsigned index>
inline u8 DMAControl::read()
{
    static_assert(index == 2 || index == 3);

    u8 byte = 0;
    switch (index)
    {
    case 2:
        byte |= dst_adjust << 5;
        byte |= src_adjust << 7;
        break;

    case 3:
        byte |= src_adjust >> 1;
        byte |= repeat     << 1;
        byte |= word       << 2;
        byte |= drq        << 3;
        byte |= timing     << 4;
        byte |= irq        << 6;
        byte |= enable     << 7;
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
        dst_adjust = bits<5, 2>(byte);
        src_adjust = (src_adjust & ~0x1) | (bits<7, 1>(byte) << 0);
        break;

    case 3:
        src_adjust = (src_adjust & ~0x2) | (bits<0, 1>(byte) << 1);
        repeat     = bits<1, 1>(byte);
        word       = bits<2, 1>(byte);
        drq        = bits<3, 1>(byte);
        timing     = bits<4, 2>(byte);
        irq        = bits<6, 1>(byte);
        enable     = bits<7, 1>(byte);
        break;

    default:
        UNREACHABLE;
        break;
    }
}
