#include "sequencer.h"

#include "base/macros.h"

void Sequencer::run(int cycles_)
{
    cycles += cycles_;

    while (cycles >= kStepCycles)
    {
        sequence();

        cycles -= kStepCycles;
    }
}

void Sequencer::sequence()
{
    switch (step)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    step = (step + 1) % 8;
}
