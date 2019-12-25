#include "displaystatus.h"

#include "common/bits.h"
#include "common/macros.h"

void DisplayStatus::reset()
{
    *this = {};
}

u8 DisplayStatus::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return data[0] | (vblank << 0) | (hblank << 1) | (vmatch << 2);
    else
        return data[1];
}

void DisplayStatus::write(int index, u8 byte)
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
    data[index] = byte;
}
