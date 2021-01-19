#include "square1.h"

#include "constants.h"

void Square1::trigger()
{
    sweep.init(frequency);
    if (sweep.shift)
        updateSweep(false);

    length.init();
    envelope.init();
    updateTimer();

    enabled = envelope.isEnabled();
}

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
    if (!(sweep.enable && sweep.timer && --sweep.timer == 0))
        return;

    updateSweep(true);
    updateSweep(false);

    sweep.init();
}

void Square1::tickLength()
{
    length.tick();

    enabled &= length.isEnabled();
}

void Square1::tickEnvelope()
{
    envelope.tick();

    enabled &= envelope.isEnabled();
}

void Square1::updateTimer()
{
    constexpr auto kWaveLength = 8;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveLength;
}

void Square1::updateSweep(bool writeback)
{
    uint freq = sweep.shadow + (sweep.negate ? -1 : 1) * (sweep.shadow >> sweep.shift);
    if  (freq > 2047)
    {
        enabled = false;
    }
    else if (writeback && sweep.shift)
    {
        frequency    = freq;
        sweep.shadow = freq;

        updateTimer();
    }
}
