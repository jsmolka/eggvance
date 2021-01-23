#include "square2.h"

#include "constants.h"

void Square2::init()
{
    length.init();
    envelope.init();

    enabled = envelope.enabled();

    updateTimer();
}

void Square2::tick()
{
    if (!(timer && --timer == 0))
        return;

    constexpr auto kWaves = 0b00111111'00001111'00000011'00000001;

    sample = (kWaves >> (8 * pattern + step)) & 0x1;
    sample *= envelope.volume;

    step = (step + 1) % 8;

    updateTimer();
}

void Square2::tickLength()
{
    length.tick();

    enabled &= length.enabled();
}

void Square2::tickEnvelope()
{
    envelope.tick();

    enabled &= envelope.enabled();
}

void Square2::updateTimer()
{
    constexpr auto kWaveBits  = 8;
    constexpr auto kFrequency = 131072;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveBits;
}
