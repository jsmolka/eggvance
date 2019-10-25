#include "timercontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void TimerControl::reset()
{
    *this = {};
}

u8 TimerControl::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
    case 1:
        byte = byteArray(data)[index];
        break;

    case 2:
        byte = bytes[2] & ~0x80;
        byte |= enabled << 7;
        break;

    case 3:
        byte = bytes[3];
        break;
    }
    return byte;
}

void TimerControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    switch (index)
    {
    case 0:
    case 1:
        byteArray(initial)[index] = byte;
        break;

    case 2:
        prescaler = bits<0, 2>(byte);
        cascade   = bits<2, 1>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
        break;
    }
    bytes[index] = byte;
}
