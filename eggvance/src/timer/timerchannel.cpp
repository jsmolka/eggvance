#include "timerchannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "arm/constants.h"
#include "scheduler/scheduler.h"

inline constexpr u64 kOverflow = 0x1'0000;

TimerChannel::TimerChannel(uint id)
    : id(id)
    , count(*this)
    , control(*this)
{
    events.run = [this](u64 late)
    {
        doRun(late);
    };

    events.start = [this](u64 late)
    {
        doStart(late);
    };
}

void TimerChannel::start()
{
    SHELL_ASSERT(!events.start.scheduled());

    if (events.start.scheduled())
        return;

    scheduler.remove(events.run);
    scheduler.add(events.start, 2);
}

void TimerChannel::update()
{
    counter  = control.prescaler * (count.counter - initial);
    overflow = control.prescaler * (kOverflow - initial);

    schedule();
}

void TimerChannel::run(u64 ticks)
{
    SHELL_ASSERT(!events.start.scheduled());

    if (events.start.scheduled())
        return;

    counter += ticks;
    
    if (counter >= overflow)
    {
        if (control.irq)
            arm.raise(kIrqTimer0 << id);

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

void TimerChannel::doRun(u64 late)
{
    run();
    schedule();
}

void TimerChannel::doStart(u64 late)
{
    SHELL_ASSERT(control.enabled);

    if (!control.enabled)
        return;

    since    = scheduler.now - late;
    counter  = 0;
    initial  = count.initial;
    overflow = control.prescaler * (kOverflow - initial);

    if (control.enabled && !control.cascade)
        run(late);

    schedule();
}

void TimerChannel::schedule()
{
    SHELL_ASSERT(!events.start.scheduled());

    scheduler.remove(events.run);

    if (!control.enabled || control.cascade)
        return;
    
    scheduler.add(events.run, overflow - counter);
}
