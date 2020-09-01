#include "dma.h"

#include "arm/arm.h"

Dma::Dma()
{
    for (auto& channel : channels)
    {
        channel.control.emit_immediate = [&]() {
            emit(channel, kTimingImmediate);
        };
    }
}

void Dma::run(int& cycles)
{
    while (active && cycles > 0)
    {
        active->run(cycles);

        if (!active->running)
        {
            active = nullptr;
            arm.state &= ~Arm::kStateDma;

            for (auto& channel : channels)
            {
                if (channel.running)
                {
                    active = &channel;
                    arm.state |= Arm::kStateDma;
                    break;
                }
            }
        }
    }
}

void Dma::broadcast(Dma::Timing timing)
{
    for (auto& channel : channels)
    {
        emit(channel, timing);
    }
}

void Dma::emit(DmaChannel& channel, Dma::Timing timing)
{
    if (!channel.running
        && channel.control.enable
        && channel.control.timing == timing)
    {
        channel.start();

        if (!active || channel.id < active->id)
        {
            active = &channel;
            arm.state |= Arm::kStateDma;
        }
    }
}
