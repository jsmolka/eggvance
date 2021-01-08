#include "apu.h"

#include <shell/utility.h>

#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"

Apu::Apu()
{
    dmacnt.channels[0].clear_fifo = [&]() { fifo[0].clear(); };
    dmacnt.channels[1].clear_fifo = [&]() { fifo[1].clear(); };

    if (config.bios_skip)
    {
        soundbias.value = 0x0200;
    }
}

void Apu::sample()
{
    s16 sample_l = soundbias.level - 0x200;
    s16 sample_r = soundbias.level - 0x200;

    if (dmacnt.channels[0].enable_l) sample_l += fifo[0].sample << dmacnt.channels[0].volume;
    if (dmacnt.channels[1].enable_l) sample_l += fifo[1].sample << dmacnt.channels[1].volume;
    if (dmacnt.channels[0].enable_r) sample_r += fifo[0].sample << dmacnt.channels[0].volume;
    if (dmacnt.channels[1].enable_r) sample_r += fifo[1].sample << dmacnt.channels[1].volume;

    sample_l = std::clamp<s16>(sample_l, -0x400, 0x3FF);
    sample_r = std::clamp<s16>(sample_r, -0x400, 0x3FF);

    audio_ctx.write(sample_l << 5, sample_r << 5);
}

void Apu::onTimerOverflow(uint id)
{
    constexpr Dma::Timing kRefill[2] = { Dma::Timing::FifoA, Dma::Timing::FifoB };

    for (auto [index, channel] : shell::enumerate(dmacnt.channels))
    {
        if (channel.timer != id)
            continue;

        auto& fifo = this->fifo[index];

        if (fifo.size() > 0)
        {
            fifo.sample = fifo.read();

            if (fifo.refillable())
            {
                dma.broadcast(kRefill[index]);
            }
        }
        else 
        {
            fifo.sample = 0;
        }
    }
}
