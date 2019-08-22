#include "displaystatus.h"

#include "common/utility.h"

u8 DisplayStatus::read(int index)
{
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
        byte = vcount_eval;
        break;
    }
    return byte;
}

void DisplayStatus::write(int index, u8 byte)
{
    switch (index)
    {
    case 0:
        vblank_irq = bits<3, 1>(byte);
        hblank_irq = bits<4, 1>(byte);
        vmatch_irq = bits<5, 1>(byte);
        break;

    case 1:
        vcount_eval = byte;
        break;
    }
}
