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
        auto tick_length = [&]() {
            square1.length.tick();
            square2.length.tick();
        };

        auto tick_envelope = [&]() {
            noise.envelope.tick();
            square1.envelope.tick();
            square2.envelope.tick();
        };

        auto tick_sweep = [&]() {
            square1.sweep.tick();
        };

        switch (step)
        {
        case 0:
        case 4:
            tick_length();
            break;

        case 1:
        case 3:
        case 5:
            break;

        case 2:
        case 6:
            tick_length();
            tick_sweep();
            break;

        case 7:
            tick_envelope();
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
