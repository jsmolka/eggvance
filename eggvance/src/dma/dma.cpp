#include "dma.h"

#include "arm/arm.h"
#include "arm/constants.h"

Dma::Dma()
{
    for (auto& channel : channels)
    {
        channel.control.on_write = [&](bool init)
        {
            if (init)
                channel.init();

            emit(channel, Dma::Event::Immediate);
        };
    }
}

void Dma::run()
{
    while (active && scheduler.now < arm.target)
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

void Dma::broadcast(Event event)
{
    for (auto& channel : channels)
    {
        emit(channel, event);
    }
}

void Dma::emit(DmaChannel& channel, Event event)
{
    auto matches = [](const DmaChannel& channel, Event event) -> bool
    {
        switch (event)
        {
        case Dma::Event::Immediate: return channel.control.timing == DmaControl::kTimingImmediate;
        case Dma::Event::HBlank:    return channel.control.timing == DmaControl::kTimingHBlank;
        case Dma::Event::VBlank:    return channel.control.timing == DmaControl::kTimingVBlank;
        case Dma::Event::Hdma:      return channel.control.timing == DmaControl::kTimingSpecial && channel.control.repeat && channel.id == 3;
        case Dma::Event::FifoA:     return channel.fifo && channel.latch.dad == 0x400'00A0;
        case Dma::Event::FifoB:     return channel.fifo && channel.latch.dad == 0x400'00A4;
        }
        return false;
    };

    if (channel.running || !channel.control.enabled || !matches(channel, event))
        return;

    if (!channel.start())
        return;

    if (!active || channel.id < active->id)
    {
        active = &channel;
        arm.state |= kStateDma;
    }
}
