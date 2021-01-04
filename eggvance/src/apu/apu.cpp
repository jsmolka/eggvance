#include "apu.h"

#include <mutex>
#include <shell/utility.h>

#include "dma/dma.h"
#include "base/config.h"
#include "base/stereosample.h"
#include "core/audiocontext.h"

Apu::Apu()
{
    dma_control.channels[0].clear_fifo = [&]() { fifo[0].clear(); };
    dma_control.channels[1].clear_fifo = [&]() { fifo[1].clear(); };

    if (config.bios_skip)
    {
        soundbias.value = 0x0200;
    }
}

void Apu::run(int cycles)
{
    this->cycles += cycles;

    while (this->cycles >= kSampleEveryCycles)
    {
        sample();

        this->cycles -= kSampleEveryCycles;
    }

}

void Apu::sample()
{
    s8 ssample = fifo[0].sample;
    s16 sssample = ssample << 6;

    audio_ctx.write({ sssample, sssample });
}

void Apu::tickDmaSound(int channel)
{
    if (const auto sample = fifo[channel].read())
    {
        fifo[channel].sample = *sample;

        if (fifo[channel].size() <= 16)
        {
            dma.broadcast(channel == 0 ? Dma::Timing::FifoA : Dma::Timing::FifoB);
        }
    }
    else 
    {
        fifo[channel].sample = 0;
    }
}

void Apu::onTimerOverflow(uint id)
{
    for (auto [fifo_id, channel] : shell::enumerate(dma_control.channels))
    {
        if (channel.timer == id)
            tickDmaSound(fifo_id);
    }
}
