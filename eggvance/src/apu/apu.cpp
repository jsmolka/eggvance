#include "apu.h"

#include <algorithm>
#include <shell/utility.h>

#include "constants.h"
#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"
#include "scheduler/scheduler.h"

inline constexpr auto kSampleCycles    = kCpuFrequency / 32 / 1024;
inline constexpr auto kSequencerCycles = kCpuFrequency / 512;

Apu::Apu()
{
    scheduler.add(kSequencerCycles, nullptr, &sequence<0>);
}

void Apu::run(int cycles_)
{
    while (cycles_--)
    {
        noise.tick();
        square1.tick();
        square2.tick();
        wave.tick();

        if (++cycles == kSampleCycles)
        {
            sample();

            cycles = 0;
        }
    }
}

void Apu::sample()
{
    if (!control.enabled)
        return audio_ctx.write(0, 0);

    s16 sample_l = 0;
    s16 sample_r = 0;

    Channel* channels[4] = { &square1, &square2, &wave, &noise };

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

template<uint Step>
void Apu::sequence(void*, u64)
{
    switch (Step)
    {
    case 1:
    case 3:
    case 5:
        break;

    case 2:
    case 6:
        apu.square1.tickSweep();
        [[fallthrough]];

    case 0:
    case 4:
        apu.noise.tickLength();
        apu.square1.tickLength();
        apu.square2.tickLength();
        apu.wave.tickLength();
        break;

    case 7:
        apu.noise.tickEnvelope();
        apu.square1.tickEnvelope();
        apu.square2.tickEnvelope();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    if constexpr (Step == 0 || Step == 2 || Step == 4)
        scheduler.add(2 * kSequencerCycles, nullptr, &sequence<(Step + 2) % 8>);
    else
        scheduler.add(1 * kSequencerCycles, nullptr, &sequence<(Step + 1) % 8>);
}
