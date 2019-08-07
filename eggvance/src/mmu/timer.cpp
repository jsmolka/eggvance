#include "timer.h"

#include "interrupt.h"

Timer::Timer(int id)
    : id(id)
    , next(nullptr)
{
    reset();
}

void Timer::reset()
{
    data = 0;
    initial = 0;
    counter = 0;
    control = {};
}

void Timer::emulate(int cycles)
{
    if (!control.enabled || control.cascade) 
        return;

    increment(cycles);
}

void Timer::attemptInit(int enabled)
{
    if (!control.enabled && enabled)
    {
        counter = 0;
        data = initial;
    }
}

void Timer::cascade()
{
    if (next && next->control.cascade)
        next->increment(1);
}

void Timer::interrupt()
{
    static constexpr InterruptFlag flags[4] = {
        IF_TIMER0, 
        IF_TIMER1, 
        IF_TIMER2, 
        IF_TIMER3
    };

    if (control.irq)
        Interrupt::request(flags[id]);
}

void Timer::increment(int amount)
{
    static constexpr int prescalers[4] = { 1, 64, 256, 1024 };

    if (control.prescaler != 0)
    {
        counter += amount;
        while (counter >= prescalers[control.prescaler])
        {
            if (++data == 0)
            {
                data = initial;
                cascade();
                interrupt();
            }
            counter -= prescalers[control.prescaler];
        }
    }
    else
    {
        int value = data + amount;
        if (value >= 0x10000)
        {
            int range = 0x10000 - initial;
            while (value >= 0x10000)
            {
                value -= range;
                cascade();
            }
            data = value;
            interrupt();
        }
        else
        {
            data += amount;
        }
    }
}