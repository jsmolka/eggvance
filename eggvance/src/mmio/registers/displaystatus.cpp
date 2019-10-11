#include "displaystatus.h"

#include "common/macros.h"
#include "common/utility.h"

void DisplayStatus::reset()
{
    vblank         = 0;
    hblank         = 0;
    vmatch         = 0;
    vblank_irq     = 0;
    hblank_irq     = 0;
    vmatch_irq     = 0;
    vcount_compare = 0;
}

u8 DisplayStatus::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

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
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

void DisplayStatus::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

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
        EGG_UNREACHABLE;
        break;
    }
}
