#pragma once

#include "../register.h"

class TimerControl : public Register<u16>
{
public:
    TimerControl(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16, 0, 2> prescaler;  // Prescaler value (0 = 1, 1 = 64, 2 = 256, 3 = 1024)
    BitField<u16, 2, 1> cascade;    // Timer disabled, increments if previous overflows
    BitField<u16, 6, 1> irq;        // Interrupt on overflow
    BitField<u16, 7, 1> enabled;    // Timer enabled
};
