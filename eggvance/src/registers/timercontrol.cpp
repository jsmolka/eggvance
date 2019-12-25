#include "timercontrol.h"

#include "common/bits.h"
#include "common/macros.h"

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

    static constexpr int prescalers[4] = { 1, 64, 256, 1024 };

    if (index == 0)
    {
        prescaler = bits<0, 2>(byte);
        cascade   = bits<2, 1>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);

        if (cascade)
            prescaler = 1;
        else 
            prescaler = prescalers[prescaler];
    }
    data[index] = byte;
}
