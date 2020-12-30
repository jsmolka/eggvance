#include "timer.h"

#include <algorithm>

#include "arm/arm.h"
#include "arm/constants.h"

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
        channel.control.on_write = [&](bool start) {
            if (start)
                channel.start();
            else
                channel.update();
            
            schedule();
        };
    }
}

int Timer::cycles() const
{
    int cycles = event - count;
    if (cycles < 0)
        cycles = kEventMax;

    return cycles;
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
            active.push_back(&channel);
            arm.state |= kStateTimer;

            event = std::min(event, channel.nextEvent());
        }
    }
}

void Timer::reschedule()
{
    event = kEventMax;
    for (const auto& channel : active)
        event = std::min(event, channel->nextEvent());
}

void Timer::runChannels()
{
    if (count == 0)
        return;

    for (const auto& channel : active)
        channel->run(count);

    event -= count;
    count = 0;
}
