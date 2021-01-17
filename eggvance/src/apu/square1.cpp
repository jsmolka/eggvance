#include "square1.h"

#include "constants.h"

void Square1::tick()
{
    if (!(timer && --timer == 0))
        return;

    constexpr auto kWaves = 0b00111111'00001111'00000011'00000001;

    sample = (kWaves >> (8 * pattern + step)) & 0x1;
    sample *= envelope.volume;

    step = (step + 1) % 8;

    updateTimer();
}

void Square1::tickSweep()
{
    if (!(sweep.timer && --sweep.timer == 0))
        return;

    updateSweep(true);
    updateSweep(false);

    sweep.init();
}

void Square1::tickLength()
{
    length.tick();
}

void Square1::tickEnvelope()
{
    envelope.tick();
}

void Square1::updateTimer()
{
    constexpr auto kWaveLength = 8;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveLength;
}

void Square1::updateSweep(bool writeback)
{
    uint value = sweep.shadow + (sweep.negate ? -1 : 1) * (sweep.shadow >> sweep.shift);
    if  (value < 2048)
    {
        if (writeback)
        {
            frequency    = value;
            sweep.shadow = value;
        }
    }
    else
    {
        enable = false;
    }
}
