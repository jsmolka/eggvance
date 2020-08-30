#include "timerchannel.h"

#include "irq/irqh.h"
#include "timer/constants.h"

constexpr uint kOverflow = 0x1'0000;

void TimerChannel::start()
{
    counter  = 0;
    initial  = count.initial;
    overflow = control.prescaler * (kOverflow - initial);
}

void TimerChannel::update()
{
    counter  = control.prescaler * (count.value - initial);
    overflow = control.prescaler * (kOverflow - initial);
}

void TimerChannel::run(int cycles)
{
    counter += cycles;
    
    if (counter >= overflow)
    {
        if (next && next->control.cascade)
            next->run(counter / overflow);

        if (control.irq)
            irqh.request(kIrqTimer0 << id);

        counter %= overflow;
        initial  = count.initial;
        overflow = control.prescaler * (kOverflow - initial);
    }
    count.value = counter / control.prescaler + initial;
}

uint TimerChannel::nextEvent() const
{
    if (!control.irq)
        return kEventMax;

    if (!control.cascade)
        return overflow - counter;

    if (!prev)
        return kEventMax;

    uint count = 0;
    uint event = overflow - counter;

    for (TimerChannel* timer = prev; timer; timer = timer->prev)
    {
        event *= timer->control.prescaler * (kOverflow - timer->initial);
        count += timer->counter;
    }
    return event - count;
}
