#include "noise.h"

#include "constants.h"

void Noise::init()
{
    if (small)
        lfsr = 0x40;
    else
        lfsr = 0x4000;

    length.init();
    envelope.init();

    enabled = envelope.enabled();

    updateTimer();
}

void Noise::tick()
{
    if (!(timer && --timer == 0))
        return;

    sample = lfsr & 0x1;

    lfsr >>= 1;
    if (sample)
    {
        if (small)
            lfsr ^= 0x60;
        else 
            lfsr ^= 0x6000;
    }

    sample *= envelope.volume;

    updateTimer();
}

void Noise::tickLength()
{
    length.tick();

    enabled &= length.enabled();
}

void Noise::tickEnvelope()
{
    envelope.tick();

    enabled &= envelope.enabled();
}

#include <cmath>

void Noise::updateTimer()
{
    constexpr double kFrequency = 524288;

    double r = divisor;
    double s = shift;

    if (r == 0)
        r = 0.5;

    timer = kCpuFrequency / (kFrequency / r / std::pow(2, s + 1));
}
