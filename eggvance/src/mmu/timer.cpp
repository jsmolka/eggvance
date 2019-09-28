#include "timer.h"

#include "interrupt.h"

Timer::Timer(int id, TimerControl& control, Timer* next)
    : id(id)
    , next(next)
    , control(control)
{

}

void Timer::emulate(int cycles)
{
    if (!control.enabled || control.cascade)
        return;

    increment(cycles);
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
        control.counter += amount;
        while (control.counter >= prescalers[control.prescaler])
        {
            if (++control.data == 0)
            {
                control.data = control.initial;
                cascade();
                interrupt();
            }
            control.counter -= prescalers[control.prescaler];
        }
    }
    else
    {
        int value = control.data + amount;
        if (value >= 0x10000)
        {
            int range = 0x10000 - control.initial;
            while (value >= 0x10000)
            {
                value -= range;
                cascade();
            }
            control.data = value;
            interrupt();
        }
        else
        {
            control.data += amount;
        }
    }
}