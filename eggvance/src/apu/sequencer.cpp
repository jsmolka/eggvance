#include "sequencer.h"

#include "constants.h"
#include "base/macros.h"
#include "scheduler/scheduler.h"

inline constexpr auto kSequencerCycles = kCpuFrequency / 512;;

Sequencer::Sequencer()
{
    scheduler.add(kSequencerCycles, this, &sequence<0>);
}

void Sequencer::tick()
{
    noise.tick();
    square1.tick();
    square2.tick();
    wave.tick();
}

void Sequencer::tickSweep()
{
    square1.tickSweep();
}

void Sequencer::tickLength()
{
    noise.tickLength();
    square1.tickLength();
    square2.tickLength();
    wave.tickLength();
}

void Sequencer::tickEnvelope()
{
    noise.tickEnvelope();
    square1.tickEnvelope();
    square2.tickEnvelope();
}

template<uint Step>
void Sequencer::sequence(void* data, u64)
{
    auto& self = *reinterpret_cast<Sequencer*>(data);

    switch (Step)
    {
    case 2:
    case 6:
        self.tickSweep();
        [[fallthrough]];

    case 0:
    case 4:
        self.tickLength();
        break;

    case 7:
        self.tickEnvelope();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    if constexpr (Step == 0 || Step == 2 || Step == 4)
        scheduler.add(2 * kSequencerCycles, data, &sequence<(Step + 2) % 8>);
    else
        scheduler.add(1 * kSequencerCycles, data, &sequence<(Step + 1) % 8>);
}
