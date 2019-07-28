#include "timer.h"

#include "interrupt.h"

Timer::Timer(int timer)
    : timer(timer)
{

}

void Timer::init()
{
    counter = 0;
    data = initial;
}

void Timer::emulate(int cycles)
{
    if (!control.enabled || control.cascade) 
        return;

    increment(cycles);
}

void Timer::increment(int amount)
{
    static constexpr int prescalers[4] = { 1, 64, 256, 1024 };

    static constexpr InterruptFlag flags[4] = {
        IF_TIMER0_OVERFLOW,
        IF_TIMER1_OVERFLOW,
        IF_TIMER2_OVERFLOW,
        IF_TIMER3_OVERFLOW
    };

    if (control.prescaler != 0)
    {
        counter += amount;

        while (counter >= prescalers[control.prescaler])
        {
            if (++data == 0)
            {
                data = initial;

                if (next && next->control.cascade)
                    next->increment(1);

                if (control.irq)
                    Interrupt::request(flags[timer]);
            }
            counter -= prescalers[control.prescaler];
        }
    }
    else  // Upper solution with prescaler 1 eats cpu cycles
    {
        int value = data + amount;
        if (value >= 0x10000)
        {
            int range = 0x10000 - initial;

            while (value >= 0x10000)
            {
                if (next && next->control.cascade)
                    next->increment(1);

                value -= range;
            }
            data = value;

            if (control.irq)
                Interrupt::request(flags[timer]);
        }
        else
        {
            data += amount;
        }
    }
}