#include "timer.h"

#include "irq/irqh.h"

static constexpr uint kOverflow = 0x1'0000;

Timer::Timer(uint id)
    : id(id)
{

}

void Timer::init()
{
    counter  = 0;
    initial  = io.count.initial;
    overflow = io.control.prescaler * (kOverflow - initial);
}

void Timer::update()
{
    counter  = io.control.prescaler * (io.count.value - initial);
    overflow = io.control.prescaler * (kOverflow - initial);
}

void Timer::run(int cycles)
{
    counter += cycles;
    
    if (counter >= overflow)
    {
        if (next && next->io.control.cascade)
            next->run(counter / overflow);

        if (io.control.irq)
            irqh.request(kIrqTimer0 << id);

        counter %= overflow;
        initial  = io.count.initial;
        overflow = io.control.prescaler * (kOverflow - initial);
    }
    io.count.value = counter / io.control.prescaler + initial;
}

uint Timer::nextEvent() const
{
    if (!io.control.irq)
        return 1 << 30;

    if (!io.control.cascade)
        return overflow - counter;

    if (!prev)
        return 1 << 30;

    uint count = 0;
    uint event = overflow - counter;

    for (auto timer = prev; timer; timer = timer->prev)
    {
        event *= timer->io.control.prescaler * (kOverflow - timer->initial);
        count += timer->counter;
    }
    return event - count;
}
