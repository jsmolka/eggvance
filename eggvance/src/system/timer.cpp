#include "timer.h"

#include "arm/arm.h"

Timer::Timer(int id)
{
    this->id = id;
}

void Timer::reset()
{
    data.reset();
    control.reset();

    reload   = 0;
    counter  = 0;
    overflow = 0;
}

void Timer::run(int cycles)
{
    counter += cycles;

    if (counter >= overflow)
    {
        if (next && next->control.cascade)
            next->run(counter / overflow);

        if (control.irq)
        {
            static constexpr Interrupt flags[4] = {
                Interrupt::Timer0,
                Interrupt::Timer1,
                Interrupt::Timer2,
                Interrupt::Timer3
            };
            arm.interrupt(flags[id]);
        }

        counter %= overflow;
        reload   = data.reload;
        overflow = control.prescaler * (0x10000 - reload);
    }
    data.counter = counter / control.prescaler + reload;
}

void Timer::start()
{
    counter  = 0;
    reload   = data.reload;
    overflow = control.prescaler * (0x10000 - reload);
}

void Timer::update()
{
    counter  = control.prescaler * (data.counter - reload);
    overflow = control.prescaler * (0x10000 - reload);
}

int Timer::nextOverflow() const
{
    return overflow - counter;
}
