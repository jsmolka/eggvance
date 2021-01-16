#include "square1.h"

#include "constants.h"

void Square1::tick()
{
    if (!(timer && --timer == 0))
        return;

    constexpr auto kWaves = 0b00111111'00001111'00000011'00000001;

    sample = (kWaves >> (8 * pattern + step)) & 0x1;
    sample <<= 4;

    step = (step + 1) % 8;

    updateTimer();
}

void Square1::updateTimer()
{
    constexpr auto kWaveLength = 8;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveLength;
}
