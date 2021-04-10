#include "apu.h"

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
    scheduler.insert(events.sample, kSampleCycles);
    scheduler.insert(events.sequence, kSequenceCycles);
}

void Apu::onOverflow(uint timer, uint ticks)
{
    if (!control.enabled)
        return;

    constexpr Dma::Event kEvents[2] = { Dma::Event::FifoA, Dma::Event::FifoB };

    for (auto [fifo, event] : shell::zip(fifos, kEvents))
    {
        if (fifo.timer != timer)
            continue;

        for (uint x = 0; x < ticks; ++x)
        {
            fifo.tick();
        }

        if (fifo.size() <= 16)
            dma.broadcast(event);
    }
}

template<uint kStep>
void Apu::sequence(u64 late)
{
    static_assert(!(kStep == 1 || kStep == 3 || kStep == 5));

    switch (kStep)
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
        square1.tickLength();
        square2.tickLength();
        wave.tickLength();
        noise.tickLength();
        break;

    case 7:
        square1.tickEnvelope();
        square2.tickEnvelope();
        noise.tickEnvelope();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    if constexpr (kStep == 0 || kStep == 2 || kStep == 4)
    {
        events.sequence = [this](u64 late)
        {
            sequence<(kStep + 2) % 8>(late);
        };
        scheduler.insert(events.sequence, 2 * kSequenceCycles - late);
    }
    else
    {
        events.sequence = [this](u64 late)
        {
            sequence<(kStep + 1) % 8>(late);
        };
        scheduler.insert(events.sequence, 1 * kSequenceCycles - late);
    }
}

void Apu::sample(u64 late)
{
    AudioContext::Samples samples = {};

    if (control.enabled)
    {
        Channel* channels[4] = { &square1, &square2, &wave, &noise };

        for (auto [index, channel] : shell::enumerate(channels))
        {
            if (!channel->enabled)
                continue;

            channel->tick();

            if (control.enabled_l & (1 << index)) samples[0] += channel->sample;
            if (control.enabled_r & (1 << index)) samples[1] += channel->sample;
        }

        samples[0] *= control.volume_l + 1;
        samples[1] *= control.volume_r + 1;
        samples[0] <<= 1;
        samples[1] <<= 1;
        samples[0] >>= 3 - control.volume;
        samples[1] >>= 3 - control.volume;

        for (const auto& fifo : fifos)
        {
            if (fifo.enabled_l) samples[0] += fifo.sample << fifo.volume;
            if (fifo.enabled_r) samples[1] += fifo.sample << fifo.volume;
        }

        samples[0] = std::clamp<s16>(samples[0] + apu.bias, -0x400, 0x3FF) << 5;
        samples[1] = std::clamp<s16>(samples[1] + apu.bias, -0x400, 0x3FF) << 5;
    }

    audio_ctx.write(samples);

    scheduler.insert(events.sample, kSampleCycles - late);
}
