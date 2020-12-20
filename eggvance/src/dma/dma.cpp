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

            emit(channel, DmaControl::kTimingImmediate);
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

void Dma::broadcast(DmaControl::Timing timing)
{
    for (auto& channel : channels)
    {
        emit(channel, timing);
    }
}

void Dma::emit(DmaChannel& channel, DmaControl::Timing timing)
{
    if (!channel.running
            && channel.control.enable
            && channel.control.timing == timing)
    {
        channel.start();

        if (!active || channel.id < active->id)
        {
            active = &channel;
            arm.state |= kStateDma;
        }
    }
}
