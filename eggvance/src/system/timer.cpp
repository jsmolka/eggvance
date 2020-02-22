#include "timer.h"

#include "system/irqhandler.h"

constexpr uint limit = 0x1'0000;

Timer::Timer(uint id)
    : id(id)
{
    reset();
}

void Timer::reset()
{
    data.reset();
    control.reset();

    counter  = 0;
    reload   = 0;
    overflow = 0;
}

void Timer::run(uint cycles)
{
    counter += cycles;
    
    if (counter >= overflow)
    {
        if (next && next->control.cascade)
            next->run(counter / overflow);

        if (control.irq)
            irqh.request(
                static_cast<Irq>(
                    static_cast<uint>(Irq::Timer) << id));

        counter %= overflow;
        reload   = data.reload;
        overflow = prescale(limit - reload);
    }
    data.counter = counter / control.prescaler + reload;
}

void Timer::start()
{
    counter  = 0;
    reload   = data.reload;
    overflow = prescale(limit - reload);
}

void Timer::update()
{
    counter  = prescale(data.counter - reload);
    overflow = prescale(limit - reload);
}

uint Timer::nextOverflow() const
{
    return overflow - counter;
}

uint Timer::prescale(uint value) const
{
    return control.prescaler * value;
}
