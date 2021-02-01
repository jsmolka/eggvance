#include "apu.h"

#include <algorithm>
#include <shell/utility.h>

#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"
#include "constants.h"

void Apu::run(int cycles_)
{
    constexpr auto kSampleRate   = 32 * 1024;
    constexpr auto kSampleCycles = kCpuFrequency / kSampleRate;

    sequencer.run(cycles_);

    cycles += cycles_;

    while (cycles >= kSampleCycles)
    {
        sample();

        cycles -= kSampleCycles;
    }
}

void Apu::sample()
{
    if (!control.enabled)
        return audio_ctx.write(0, 0);

    s16 sample_l = 0;
    s16 sample_r = 0;

    Channel* channels[4] = {
        &sequencer.square1,
        &sequencer.square2,
        &sequencer.wave,
        &sequencer.noise
    };

    for (auto [index, channel] : shell::enumerate(channels))
    {
        if (!channel->enabled)
            continue;

        if (control.enabled_l & (1 << index)) sample_l += channel->sample;
        if (control.enabled_r & (1 << index)) sample_r += channel->sample;
    }

    sample_l  *= control.volume_l + 1;
    sample_r  *= control.volume_r + 1;
    sample_l <<= 1;
    sample_r <<= 1;
    sample_l >>= 3 - control.volume;
    sample_r >>= 3 - control.volume;

    if (fifo[0].enabled_l) sample_l += fifo[0].sample << fifo[0].volume;
    if (fifo[0].enabled_r) sample_r += fifo[0].sample << fifo[0].volume;
    if (fifo[1].enabled_l) sample_l += fifo[1].sample << fifo[1].volume;
    if (fifo[1].enabled_r) sample_r += fifo[1].sample << fifo[1].volume;

    sample_l = std::clamp<s16>(sample_l + bias - 0x200, -0x400, 0x3FF);
    sample_r = std::clamp<s16>(sample_r + bias - 0x200, -0x400, 0x3FF);

    audio_ctx.write(sample_l << 5, sample_r << 5);
}

void Apu::onTimerOverflow(uint timer, uint times)
{
    if (!control.enabled)
        return;

    constexpr Dma::Timing kEvent[2] = { Dma::Timing::FifoA, Dma::Timing::FifoB };

    for (auto [index, fifo] : shell::enumerate(fifo))
    {
        if (fifo.timer != timer)
            continue;

        for (uint x = 0; x < times; ++x)
        {
            fifo.tick();
        }

        if (fifo.refillable())
        {
            dma.broadcast(kEvent[index]);
        }
    }
}
