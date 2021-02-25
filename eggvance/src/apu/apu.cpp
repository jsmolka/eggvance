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
    scheduler.add(kSampleCycles, nullptr, sample);
    scheduler.add(kSequencerCycles, nullptr, sequence<0>);
}

void Apu::run(int cycles_)
{
    while (cycles_--)
    {
        noise.tick();
        square1.tick();
        square2.tick();
        wave.tick();
    }
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

void Apu::sample(void*, u64 late)
{
    s16 sample_l = 0;
    s16 sample_r = 0;

    if (apu.control.enabled)
    {
        Channel* channels[4] = { &apu.square1, &apu.square2, &apu.wave, &apu.noise };

        for (auto [index, channel] : shell::enumerate(channels))
        {
            if (!channel->enabled)
                continue;

            if (apu.control.enabled_l & (1 << index)) sample_l += channel->sample;
            if (apu.control.enabled_r & (1 << index)) sample_r += channel->sample;
        }

        sample_l  *= apu.control.volume_l + 1;
        sample_r  *= apu.control.volume_r + 1;
        sample_l <<= 1;
        sample_r <<= 1;
        sample_l >>= 3 - apu.control.volume;
        sample_r >>= 3 - apu.control.volume;

        for (const auto& fifo : apu.fifo)
        {
            if (fifo.enabled_l) sample_l += fifo.sample << fifo.volume;
            if (fifo.enabled_r) sample_r += fifo.sample << fifo.volume;
        }

        sample_l = std::clamp<s16>(sample_l + apu.bias - 0x200, -0x400, 0x3FF);
        sample_r = std::clamp<s16>(sample_r + apu.bias - 0x200, -0x400, 0x3FF);
    }

    audio_ctx.write(sample_l << 5, sample_r << 5);

    scheduler.add(kSampleCycles - late, nullptr, sample);
}

template<uint Step>
void Apu::sequence(void*, u64 late)
{
    static_assert(!(Step == 1 || Step == 3 || Step == 5));

    switch (Step)
    {
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
        scheduler.add(2 * kSequencerCycles - late, nullptr, &sequence<(Step + 2) % 8>);
    else
        scheduler.add(1 * kSequencerCycles - late, nullptr, &sequence<(Step + 1) % 8>);
}
