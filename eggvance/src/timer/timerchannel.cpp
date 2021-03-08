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
    }

    count.value = counter / control.prescaler + initial;

    schedule();

    since = scheduler.now;
}

void TimerChannel::run()
{
    run(scheduler.now - since);
}

void TimerChannel::schedule()
{
    scheduler.erase(events.run);

    if (control.cascade)
        return;
    
    scheduler.addIn(events.run, overflow - counter);
}

void TimerChannel::Events::doRun(void* data, u64 late)
{
    TimerChannel& channel = *reinterpret_cast<TimerChannel*>(data);

    channel.run(scheduler.now - channel.since + late);
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
}
