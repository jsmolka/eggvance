#include "apu.h"

#include <mutex>
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
    s16 sample = (s8)fifo[0].sample << 6; 

    audio_ctx.write(sample, sample);
}

void Apu::onTimerOverflow(uint id)
{
    constexpr Dma::Timing refill[2] = { Dma::Timing::FifoA, Dma::Timing::FifoB };

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
                dma.broadcast(refill[index]);
            }
        }
        else 
        {
            fifo.sample = 0;
        }
    }
}
