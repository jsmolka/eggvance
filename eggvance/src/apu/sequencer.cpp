#include "sequencer.h"

#include "base/macros.h"
#include "constants.h"

void Sequencer::run(int cycles)
{
    while (cycles--)
        tick();
}

void Sequencer::tick()
{
    constexpr auto kSequencerCycles = kCpuFrequency / 512;

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
            square1.tickLength();
            square2.length.tick();
            break;

        case 7:
            noise.envelope.tick();
            square1.tickEnvelope();
            square2.envelope.tick();
            break;

        default:
            SHELL_UNREACHABLE;
            break;
        }

        step = (step + 1) % 8;
    }

    noise.tick();
    square1.tick();
    square2.tick();
    wave.tick();

    cycles = (cycles + 1) % kSequencerCycles;
}
