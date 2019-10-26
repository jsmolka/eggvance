#include "timercontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void TimerControl::reset()
{
    *this = {};
}

u8 TimerControl::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return (bytes[0] & ~0x80) | (enabled << 7);
    else
        return bytes[1];
}

void TimerControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        prescaler = bits<0, 2>(byte);
        cascade   = bits<2, 1>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
    }
    bytes[index] = byte;
}
