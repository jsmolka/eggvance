#include "sequencer.h"

#include "base/macros.h"

void Sequencer::run(int amount)
{
    constexpr auto kTickCycles = kCpuFrequency / kFrequency;

    cycles += amount;

    while (cycles >= kTickCycles)
    {
        tick();

        cycles -= kTickCycles;
    }
}

void Sequencer::tick()
{
    constexpr auto kFrequencyEnvelope = 64;
    constexpr auto kFrequencySweep    = 128;
    constexpr auto kFrequencyLength   = 256;

    if (step % kFrequencyEnvelope == 0)
    {
        noise.envelope.tick();
        square1.envelope.tick();
        square2.envelope.tick();

        if (step % kFrequencySweep == 0)
        {
            square1.sweep.tick();

            if (step % kFrequencyLength == 0)
            {
                // Tick length
            }
        }
    }

    //noise.tick();
    //square1.tick();
    square2.tick();
    //wave.tick();

    step = (step + 1) % kFrequency;
}
