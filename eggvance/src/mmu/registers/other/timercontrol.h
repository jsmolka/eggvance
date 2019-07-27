#pragma once

struct TimerControl
{
    int prescaler;  // Prescaler value (0 = 1, 1 = 64, 2 = 256, 3 = 1024)
    int cascade;    // Timer disabled, increments if previous overflows
    int irq;        // Interrupt on overflow
    int enabled;    // Timer enabled
};
