#pragma once

#include "common/integer.h"

class Timer
{
public:
    Timer(int number);

    void reset();

    void emulate(int cycles);
    void attemptInit(int enabled);

    struct TimerControl
    {
        int prescaler;  // Prescaler value (0 = 1, 1 = 64, 2 = 256, 3 = 1024)
        int cascade;    // Timer disabled, increments if previous overflows
        int irq;        // Interrupt on overflow
        int enabled;    // Enabled
    } control;

    union
    {
        u8  data_b[2];
        u16 data;
    };
    union
    {
        u8  initial_b[2];
        u16 initial;
    };
    Timer* next;

private:
    int number;
    int counter;

    void cascade();
    void interrupt();
    void increment(int amount);
};
