#include "apu.h"

#include <shell/utility.h>

#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"

Apu::Apu()
{
    direct_sound.channels[0].clear_fifo = [&]() { fifo[0].clear(); };
    direct_sound.channels[1].clear_fifo = [&]() { fifo[1].clear(); };
}

void Apu::run(int cycles_)
{
    constexpr auto kCpuFrequency = 16 * 1024 * 1024;
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
    s16 sample_l = sound_bias.level - 0x200;
    s16 sample_r = sound_bias.level - 0x200;

    sample_l += sequencer.square1.sample;
    sample_r += sequencer.square1.sample;

    if (sound_enable.enable)
    {
        if (direct_sound.channels[0].enable_l) sample_l += fifo[0].sample << direct_sound.channels[0].volume;
        if (direct_sound.channels[1].enable_l) sample_l += fifo[1].sample << direct_sound.channels[1].volume;
        if (direct_sound.channels[0].enable_r) sample_r += fifo[0].sample << direct_sound.channels[0].volume;
        if (direct_sound.channels[1].enable_r) sample_r += fifo[1].sample << direct_sound.channels[1].volume;
    }

    sample_l = std::clamp<s16>(sample_l, -0x400, 0x3FF);
    sample_r = std::clamp<s16>(sample_r, -0x400, 0x3FF);

    audio_ctx.write(sample_l << 5, sample_r << 5);
}

void Apu::onTimerOverflow(uint id, uint times)
{
    if (!sound_enable.enable)
        return;

    constexpr Dma::Timing kRefill[2] = { Dma::Timing::FifoA, Dma::Timing::FifoB };

    for (auto [index, channel] : shell::enumerate(direct_sound.channels))
    {
        if (channel.timer != id)
            continue;

        auto& fifo = this->fifo[index];

        for (uint x = 0; x < times; ++x)
        {
            fifo.sample = fifo.size() > 0
                ? fifo.read()
                : 0;
        }

        if (fifo.refillable())
        {
            dma.broadcast(kRefill[index]);
        }
    }
}
