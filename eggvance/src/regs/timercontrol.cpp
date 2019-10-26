#include "timercontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void TimerControl::reset()
{
    *this = {};
}

u8 TimerControl::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void TimerControl::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        prescaler = bits<0, 2>(byte);
        cascade   = bits<2, 1>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
    }
    data[index] = byte;
}
