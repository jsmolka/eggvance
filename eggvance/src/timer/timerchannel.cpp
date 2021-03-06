#include "timerchannel.h"

#include "apu/apu.h"
#include "arm/arm.h"
#include "arm/constants.h"
#include "scheduler/scheduler.h"

constexpr u64 kOverflow = 0x1'0000;

TimerChannel::TimerChannel(uint id)
    : id(id), count(*this), control(*this)
{

}

void TimerChannel::start()
{
    delay    = 2;
    since    = scheduler.now;
    counter  = 0;
    initial  = count.initial;
    overflow = control.prescaler * (kOverflow - initial);

    schedule();
}

void TimerChannel::update()
{
    counter  = control.prescaler * (count.value - initial);
    overflow = control.prescaler * (kOverflow - initial);

    schedule();
}

// Todo: not cycles in cascading timers
void TimerChannel::run(u64 times)
{
    SHELL_ASSERT(times < 0x20'000);

    while (delay && times)
    {
        --delay;
        --times;
    }

    counter += times;
    
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
    since = scheduler.now;
}

void TimerChannel::run()
{
    run(scheduler.now - since);
}

void TimerChannel::schedule()
{
    if (event)
        scheduler.remove({ event, this, &eventRun });

    event = reschedule();
}

u64 TimerChannel::reschedule()
{
    if (!control.irq && id > 1)
        return 0;

    if (!control.cascade)
        return scheduler.add(overflow - counter + delay, this, &eventRun);

    if (!pred)
        return 0;


    u64 count = 0;
    u64 event = overflow - counter;

    for (auto* channel = pred; channel; channel = channel->pred)
    {
        event *= channel->control.prescaler * (kOverflow - channel->initial);
        count += channel->counter;
    }

    return scheduler.add(event - count + delay, this, &eventRun);
}

void TimerChannel::eventRun(void* data, u64 late)
{
    TimerChannel& channel = *reinterpret_cast<TimerChannel*>(data);

    channel.run(scheduler.now - channel.since + late);
    channel.event = channel.reschedule();
}

void TimerChannel::eventStart(void* data, u64 late)
{
    // Todo
}
