#include "dma.h"

#include "arm/arm.h"

void Dma::run()
{
    while (active && scheduler.now < arm.target)
    {
        active->run();

        if (!active->running)
        {
            active = nullptr;
            arm.state &= ~State::Dma;

            for (auto& channel : channels)
            {
                if (channel.running)
                {
                    active = &channel;
                    arm.state |= State::Dma;
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
    if (!channel.control.enabled || channel.running || !triggers(channel, event))
        return;

    if (!channel.start())
        return;

    if (!active || channel.id < active->id)
    {
        active = &channel;
        arm.state |= State::Dma;
    }
}

bool Dma::triggers(const DmaChannel& channel, Event event)
{
    switch (event)
    {
    case Dma::Event::Immediate: return channel.control.timing == DmaControl::Timing::Immediate;
    case Dma::Event::HBlank:    return channel.control.timing == DmaControl::Timing::HBlank;
    case Dma::Event::VBlank:    return channel.control.timing == DmaControl::Timing::VBlank;
    case Dma::Event::FifoA:     return channel.control.timing == DmaControl::Timing::Special && channel.latch.fifo && channel.latch.dad.fifoA();
    case Dma::Event::FifoB:     return channel.control.timing == DmaControl::Timing::Special && channel.latch.fifo && channel.latch.dad.fifoB();
    case Dma::Event::Hdma:      return channel.control.timing == DmaControl::Timing::Special && channel.id == 3 && channel.control.repeat;
    }
    return false;
}
