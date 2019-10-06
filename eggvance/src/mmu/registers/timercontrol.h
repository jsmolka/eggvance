#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct TimerControl
{
    void reset();

    template<unsigned index>
    inline u8 read();

    template<unsigned index>
    inline void write(u8 byte);

    int prescaler;  // Prescaler value (0 = 1, 1 = 64, 2 = 256, 3 = 1024)
    int cascade;    // Timer disabled, increments if previous overflows
    int irq;        // Interrupt on overflow
    int enabled;    // Enabled

    u16 data;     // Data
    u16 initial;  // Initial value
    int counter;  // Internal counter
};

template<unsigned index>
inline u8 TimerControl::read()
{
    static_assert(index <= 1);

    return bytes(&data)[index];
}

template<unsigned index>
inline void TimerControl::write(u8 byte)
{
    static_assert(index <= 2);

    switch (index)
    {
    case 0:
    case 1:
        bytes(&initial)[index] = byte;
        break;

    case 2:
        if (!enabled && byte & 0x80)
        {
            counter = 0;
            data = initial;
        }
        prescaler = bits<0, 2>(byte);
        cascade   = bits<2, 1>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
