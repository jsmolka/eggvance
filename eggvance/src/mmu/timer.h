#pragma once

#include "common/integer.h"

class Timer
{
public:
    Timer(int timer);

    void init();
    void emulate(int cycles);

    Timer* next = nullptr;

    u16 data;
    int initial;

    struct TimerControl
    {
        int prescaler;  // Prescaler value (0 = 1, 1 = 64, 2 = 256, 3 = 1024)
        int cascade;    // Timer disabled, increments if previous overflows
        int irq;        // Interrupt on overflow
        int enabled;    // Timer enabled
    } control;

private:
    int timer;
    int counter;

    void increment(int amount);
};
