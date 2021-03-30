#include "timerchannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "scheduler/scheduler.h"

inline constexpr auto kOverflow = 0x1'0000;

TimerChannel::TimerChannel(uint id)
    : id(id), count(*this), control(*this)
{
    events.run = [this](u64 late)
    {
        run();
        schedule();
    };

    events.start = [this](u64 late)
    {
        if (!control.enabled)
            return;

        since    = scheduler.now - late;
        counter  = 0;
        initial  = count.initial;
        overflow = control.prescaler * (kOverflow - initial);

        if (control.runnable())
            run(late);

        schedule();
    };
}

void TimerChannel::start()
{
    scheduler.remove(events.run);
    scheduler.insert(events.start, 2);
}

void TimerChannel::update()
{
    counter  = control.prescaler * (count.counter - initial);
    overflow = control.prescaler * (kOverflow - initial);

    schedule();
}

void TimerChannel::run(u64 ticks)
{
    if (events.start.scheduled())
        return;

    counter += ticks;
    
    if (counter >= overflow)
    {
        if (control.irq)
            arm.raise(Irq::Timer << id, counter - overflow);

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
    scheduler.remove(events.run);

    if (!control.runnable() || events.start.scheduled())
        return;
    
    scheduler.insert(events.run, overflow - counter);
}
