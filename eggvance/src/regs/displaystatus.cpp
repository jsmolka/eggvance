#include "displaystatus.h"

#include "common/macros.h"
#include "common/utility.h"

void DisplayStatus::reset()
{
    *this = {};
}

u8 DisplayStatus::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        u8 byte = bytes[0] & ~0x7;
        byte |= vblank << 0;
        byte |= hblank << 1;
        byte |= vmatch << 2;
        return byte;
    }
    else
    {
        return bytes[1];
    }
}

void DisplayStatus::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        vblank_irq = bits<3, 1>(byte);
        hblank_irq = bits<4, 1>(byte);
        vmatch_irq = bits<5, 1>(byte);
    }
    else
    {
        vcompare = byte;
    }
    bytes[index] = byte;
}
