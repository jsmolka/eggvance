#include "timer.h"

Timer::Timer(TimerControl& control, TimerData& data)
    : control(control)
    , data(data)
{

}

void Timer::init()
{
    data = initial;
}

void Timer::step()
{
    if (!control.enabled || control.cascade) 
        return;

    increment();
}

bool Timer::requestInterrupt() const
{
    return control.irq && overflowed();
}

void Timer::increment()
{
    static int prescalers[4] = { 1, 64, 256, 1024 };

    if (++counter == prescalers[control.prescaler]) 
    {
        counter = 0;
        data = data + 1;
        if (data == 0) 
        {
            data = initial;
            if (next && next->control.cascade)
                next->increment();
        }
    }
}

bool Timer::overflowed() const
{
    return control.cascade
        ? (counter == 0 && data == initial && prev && prev->overflowed())
        : (counter == 0 && data == initial);
}
