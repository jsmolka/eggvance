#include "dma.h"

#include "arm/arm.h"
#include "arm/constants.h"

Dma::Dma()
{
    for (auto& channel : channels)
    {
        channel.control.on_write = [&](bool reload) {
            if (reload)
                channel.reload();

            emit(channel, Dma::Timing::Immediate);
        };
    }
}

void Dma::run()
{
    while (active && arm.cycles > 0)
    {
        active->run();

        if (!active->running)
        {
            active = nullptr;
            arm.state &= ~kStateDma;

            for (auto& channel : channels)
            {
                if (channel.running)
                {
                    active = &channel;
                    arm.state |= kStateDma;
                    break;
                }
            }
        }
    }
}

void Dma::broadcast(Timing timing)
{
    for (auto& channel : channels)
    {
        emit(channel, timing);
    }
}

void Dma::emit(DmaChannel& channel, Timing timing)
{
    bool matches = std::invoke([&]() {
        switch (timing)
        {
        case Dma::Timing::Immediate: return channel.control.timing == DmaControl::kTimingImmediate;
        case Dma::Timing::VBlank:    return channel.control.timing == DmaControl::kTimingVBlank;
        case Dma::Timing::HBlank:    return channel.control.timing == DmaControl::kTimingHBlank;
        case Dma::Timing::Audio:     return channel.control.timing == DmaControl::kTimingSpecial && (channel.id == 1 || channel.id == 2);
        case Dma::Timing::Video:     return channel.control.timing == DmaControl::kTimingSpecial && (channel.id == 3);
        }
        return false;
    });

    if (channel.running || !channel.control.enable || !matches)
        return;

    channel.start();

    if (!active || channel.id < active->id)
    {
        active = &channel;
        arm.state |= kStateDma;
    }
}
