#include "timer.h"

#include <algorithm>

#include "arm/arm.h"

Timer::Timer()
{
    channels[1].prev = &channels[0];
    channels[2].prev = &channels[1];
    channels[3].prev = &channels[2];

    channels[0].next = &channels[1];
    channels[1].next = &channels[2];
    channels[2].next = &channels[3];

    for (auto& channel : channels)
    {
        channel.count.run_channels = std::bind(&Timer::runChannels, this);
        channel.control.run_channels = std::bind(&Timer::runChannels, this);
        channel.control.update_channel = [&](bool start) {
            if (start)
                channel.start();
            else
                channel.update();
            
            schedule();
        };
    }
}

void Timer::run(int cycles)
{
    count += cycles;

    if (count >= event)
    {
        runChannels();
        reschedule();
    }
}

void Timer::runUntilIrq(int& cycles)
{
    int rem = event - count;
    if (rem < cycles)
    {
        run(rem);
        cycles -= rem;
    }
    else
    {
        run(cycles);
        cycles = 0;
    }
}

void Timer::schedule()
{
    active.clear();
    arm.state &= ~kStateTimer;

    event = kEventMax;
    for (auto& channel : channels)
    {
        if (channel.control.enable && !channel.control.cascade)
        {
            active.push_back(std::ref(channel));
            arm.state |= kStateTimer;

            event = std::min(event, channel.nextEvent());
        }
    }
}

void Timer::reschedule()
{
    event = kEventMax;
    for (TimerChannel& channel : active)
        event = std::min(event, channel.nextEvent());
}

void Timer::runChannels()
{
    for (TimerChannel& channel : active)
        channel.run(count);

    event -= count;
    count = 0;
}
