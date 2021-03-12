#include "timerchannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "arm/constants.h"
#include "scheduler/scheduler.h"

inline constexpr u64 kOverflow = 0x1'0000;

TimerChannel::TimerChannel(uint id)
    : id(id), count(*this), control(*this)
{
    events.run = [this](u64 late)
    {
        run(scheduler.now - since + late);
        schedule();
    };

    events.start = [this](u64 late)
    {
        since    = scheduler.now - late;
        counter  = 0;
        initial  = count.initial;
        overflow = control.prescaler * (kOverflow - initial);

        if (control.cascade == 0)
            run(late);

        schedule();
    };
}

void TimerChannel::start()
{
    if (events.start.when)
        return;

    scheduler.dequeue(events.run);
    scheduler.queueIn(events.start, 2);
}

void TimerChannel::update()
{
    counter  = control.prescaler * (count.counter - initial);
    overflow = control.prescaler * (kOverflow - initial);

    schedule();
}

void TimerChannel::run(u64 ticks)
{
    if (!control.enabled || events.start.when)
        return;

    counter += ticks;
    
    if (counter >= overflow)
    {
        if (control.irq)
            arm.raise(Irq::kTimer0 << id);

        if (next && next->control.cascade)
            next->run(counter / overflow);

        if (id <= 1)
            apu.onOverflow(id, counter / overflow);

        counter %= overflow;
        initial  = count.initial;
        overflow = control.prescaler * (kOverflow - initial);
    }

    count.counter = counter / control.prescaler + initial;
    since = scheduler.now;
}

void TimerChannel::run()
{
    run(scheduler.now - since);
}

void TimerChannel::schedule()
{
    scheduler.dequeue(events.run);

    if (control.cascade)
        return;
    
    scheduler.queueIn(events.run, overflow - counter);
}
