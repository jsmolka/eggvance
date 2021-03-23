#include "apu.h"

#include <algorithm>

#include "base/config.h"
#include "base/constants.h"
#include "dma/dma.h"
#include "frontend/audiocontext.h"
#include "scheduler/scheduler.h"

inline constexpr auto kSampleCycles   = kCpuFrequency / kSampleRate;
inline constexpr auto kSequenceCycles = kCpuFrequency / 512;

Apu::Apu()
{
    events.sequence = [this](u64 late)
    {
        sequence<0>(late);
    };

    events.sample = [this](u64 late)
    {
        sample(late);
    };
}

void Apu::init()
{
    scheduler.add(events.sample, kSampleCycles);
    scheduler.add(events.sequence, kSequenceCycles);
}

void Apu::onOverflow(uint timer, uint times)
{
    if (!control.enabled)
        return;

    constexpr Dma::Event kEvent[2] = { Dma::Event::FifoA, Dma::Event::FifoB };

    for (auto [fifo, event] : shell::zip(fifo, kEvent))
    {
        if (fifo.timer != timer)
            continue;

        for (uint x = 0; x < times; ++x)
        {
            fifo.tick();
        }

        if (fifo.size() <= 16)
            dma.broadcast(event);
    }
}

template<uint Step>
void Apu::sequence(u64 late)
{
    static_assert(!(Step == 1 || Step == 3 || Step == 5));

    switch (Step)
    {
    case 1:
    case 3:
    case 5:
        break;

    case 2:
    case 6:
        square1.tickSweep();
        [[fallthrough]];

    case 0:
    case 4:
        noise.tickLength();
        square1.tickLength();
        square2.tickLength();
        wave.tickLength();
        break;

    case 7:
        noise.tickEnvelope();
        square1.tickEnvelope();
        square2.tickEnvelope();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    if constexpr (Step == 0 || Step == 2 || Step == 4)
    {
        events.sequence = [this](u64 late)
        {
            sequence<(Step + 2) % 8>(late);
        };
        scheduler.add(events.sequence, 2 * kSequenceCycles - late);
    }
    else
    {
        events.sequence = [this](u64 late)
        {
            sequence<(Step + 1) % 8>(late);
        };
        scheduler.add(events.sequence, 1 * kSequenceCycles - late);
    }
}

void Apu::sample(u64 late)
{
    s16 sample_l = 0;
    s16 sample_r = 0;

    if (control.enabled)
    {
        Channel* channels[4] = { &square1, &square2, &wave, &noise };

        for (auto [index, channel] : shell::enumerate(channels))
        {
            if (!channel->enabled)
                continue;

            channel->tick();

            if (control.enabled_l & (1 << index)) sample_l += channel->sample;
            if (control.enabled_r & (1 << index)) sample_r += channel->sample;
        }

        sample_l  *= control.volume_l + 1;
        sample_r  *= control.volume_r + 1;
        sample_l <<= 1;
        sample_r <<= 1;
        sample_l >>= 3 - control.volume;
        sample_r >>= 3 - control.volume;

        for (const auto& fifo : fifo)
        {
            if (fifo.enabled_l) sample_l += fifo.sample << fifo.volume;
            if (fifo.enabled_r) sample_r += fifo.sample << fifo.volume;
        }

        sample_l = std::clamp<s16>(sample_l + apu.bias - 0x200, -0x400, 0x3FF);
        sample_r = std::clamp<s16>(sample_r + apu.bias - 0x200, -0x400, 0x3FF);
    }

    audio_ctx.write(sample_l << 5, sample_r << 5);

    scheduler.add(events.sample, kSampleCycles - late);
}
