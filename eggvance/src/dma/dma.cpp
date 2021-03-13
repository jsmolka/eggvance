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

void Dma::broadcast(Timing timing)
{
    for (auto& channel : channels)
    {
        emit(channel, timing);
    }
}

void Dma::emit(DmaChannel& channel, Timing timing)
{
    auto matches = [](const DmaChannel& channel, Timing timing) {
        switch (timing)
        {
        case Dma::Timing::Immediate: return channel.control.timing == DmaControl::Timing::kImmediate;
        case Dma::Timing::VBlank:    return channel.control.timing == DmaControl::Timing::kVBlank;
        case Dma::Timing::HBlank:    return channel.control.timing == DmaControl::Timing::kHBlank;
        case Dma::Timing::Video:     return channel.control.timing == DmaControl::Timing::kSpecial && (channel.id == 3);
        case Dma::Timing::FifoA:     return channel.fifo && channel.internal.dst_addr == 0x400'00A0;
        case Dma::Timing::FifoB:     return channel.fifo && channel.internal.dst_addr == 0x400'00A4;
        }
        return false;
    };

    if (channel.running || !channel.control.enable || !matches(channel, timing))
        return;

    if (!channel.start())
        return;

    if (!active || channel.id < active->id)
    {
        active = &channel;
        arm.state |= kStateDma;
    }
}
