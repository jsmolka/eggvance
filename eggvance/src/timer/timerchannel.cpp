#include "timerchannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "arm/constants.h"
#include "scheduler/scheduler.h"

constexpr u64 kOverflow = 0x1'0000;

TimerChannel::TimerChannel(uint id)
    : id(id), count(*this), control(*this)
{
    events.run.data = this;
    events.run.callback = &Events::doRun;

    events.start.data = this;
    events.start.callback = &Events::doStart;
}

void TimerChannel::start()
{
    scheduler.erase(events.run);
    scheduler.addIn(events.start, 2);
}

void TimerChannel::update()
{
    counter  = control.prescaler * (count.value - initial);
    overflow = control.prescaler * (kOverflow - initial);

    schedule();
}

void TimerChannel::run(u64 ticks)
{
    SHELL_ASSERT(ticks < 0xFFFF'FFFF);
    SHELL_ASSERT(events.start.when == 0);

    counter += ticks;
    
    if (counter >= overflow)
    {
        if (control.irq)
            arm.raise(kIrqTimer0 << id);

        if (succ && succ->control.cascade)
            succ->run(counter / overflow);

        if (id <= 1)
            apu.onOverflow(id, counter / overflow);

        counter %= overflow;
        initial  = count.initial;
        overflow = control.prescaler * (kOverflow - initial);

        schedule();
    }

    since = scheduler.now;
    count.value = counter / control.prescaler + initial;
}

void TimerChannel::run()
{
    run(scheduler.now - since);
}

void TimerChannel::schedule()
{
    scheduler.erase(events.run);

    if (!control.cascade)
        return scheduler.addIn(events.run, overflow - counter);

    if (!pred)
        return;

    u64 count = 0;
    u64 event = overflow - counter;

    for (auto* channel = pred; channel; channel = channel->pred)
    {
        event *= channel->control.prescaler * (kOverflow - channel->initial);
        count += channel->counter;
    }

    scheduler.addIn(events.run, event - count);
}

void TimerChannel::Events::doRun(void* data, u64 late)
{
    TimerChannel& channel = *reinterpret_cast<TimerChannel*>(data);

    channel.run(scheduler.now - channel.since + late);
    channel.schedule();
}

void TimerChannel::Events::doStart(void* data, u64 late)
{
    TimerChannel& channel = *reinterpret_cast<TimerChannel*>(data);

    channel.since    = scheduler.now - late;
    channel.counter  = 0;
    channel.initial  = channel.count.initial;
    channel.overflow = channel.control.prescaler * (kOverflow - channel.initial);

    if (channel.control.cascade == 0)
        channel.run(late);

    channel.schedule();
}
