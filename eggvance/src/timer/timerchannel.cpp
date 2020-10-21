#include "timerchannel.h"

#include "constants.h"
#include "arm/arm.h"
#include "arm/constants.h"

constexpr uint kOverflow = 0x1'0000;

TimerChannel::TimerChannel(uint id)
    : id(id)
{

}

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
        if (control.irq)
            arm.raise(kIrqTimer0 << id);

        if (next && next->control.cascade)
            next->run(counter / overflow);

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

    for (auto* channel = prev; channel; channel = channel->prev)
    {
        event *= channel->control.prescaler * (kOverflow - channel->initial);
        count += channel->counter;
    }
    return event - count;
}
