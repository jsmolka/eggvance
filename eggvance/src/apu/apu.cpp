#include "apu.h"

#include <shell/utility.h>

#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"
#include "constants.h"

Apu::Apu()
{
    direct_sound.channels[0].clear_fifo = [&]() { fifo[0].clear(); };
    direct_sound.channels[1].clear_fifo = [&]() { fifo[1].clear(); };
}

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
    if (!sound_enable.enable)
        return audio_ctx.write(0, 0);

    s16 sample_l = 0;
    s16 sample_r = 0;

    if (sequencer.square1.enabled)
    {
        if (psg_sound.enable_l[0]) sample_l += sequencer.square1.sample;
        if (psg_sound.enable_r[0]) sample_r += sequencer.square1.sample;
    }
    
    if (sequencer.square2.enabled)
    {
        if (psg_sound.enable_l[1]) sample_l += sequencer.square2.sample;
        if (psg_sound.enable_r[1]) sample_r += sequencer.square2.sample;
    }

    if (sequencer.wave.enabled)
    {
        if (psg_sound.enable_l[2]) sample_l += sequencer.wave.sample;
        if (psg_sound.enable_r[2]) sample_r += sequencer.wave.sample;
    }

    if (sequencer.noise.enabled)
    {
        if (psg_sound.enable_l[3]) sample_l += sequencer.noise.sample;
        if (psg_sound.enable_r[3]) sample_r += sequencer.noise.sample;
    }

    sample_l  *= psg_sound.volume_l + 1;
    sample_r  *= psg_sound.volume_r + 1;
    sample_l <<= 1;
    sample_r <<= 1;
    sample_l >>= 3 - direct_sound.volume;
    sample_r >>= 3 - direct_sound.volume;

    if (direct_sound.channels[0].enable_l) sample_l += fifo[0].sample << direct_sound.channels[0].volume;
    if (direct_sound.channels[0].enable_r) sample_r += fifo[0].sample << direct_sound.channels[0].volume;
    if (direct_sound.channels[1].enable_l) sample_l += fifo[1].sample << direct_sound.channels[1].volume;
    if (direct_sound.channels[1].enable_r) sample_r += fifo[1].sample << direct_sound.channels[1].volume;

    sample_l = sound_bias.finalize(sample_l);
    sample_r = sound_bias.finalize(sample_r);

    audio_ctx.write(sample_l << 5, sample_r << 5);
}

void Apu::onTimerOverflow(uint id, uint times)
{
    if (!sound_enable.enable)
        return;

    constexpr Dma::Timing kEvent[2] = { Dma::Timing::FifoA, Dma::Timing::FifoB };

    for (auto [index, channel] : shell::enumerate(direct_sound.channels))
    {
        if (channel.timer != id)
            continue;

        for (uint x = 0; x < times; ++x)
        {
            fifo[index].tick();
        }

        if (fifo[index].refillable())
        {
            dma.broadcast(kEvent[index]);
        }
    }
}
