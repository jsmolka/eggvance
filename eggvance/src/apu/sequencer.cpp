#include "sequencer.h"

#include "constants.h"
#include "base/macros.h"

void Sequencer::run(int cycles)
{
    while (cycles--)
        tick();
}

void Sequencer::tick()
{
    constexpr auto kSequencerCycles = kCpuFrequency / 512;

    noise.tick();
    square1.tick();
    square2.tick();
    wave.tick();

    if (cycles == 0)
    {
        switch (step)
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

        step = (step + 1) % 8;
    }

    cycles = (cycles + 1) % kSequencerCycles;
}
