#include "square1.h"

#include "constants.h"

void Square1::init()
{
    length.init();
    envelope.init();
    sweep.init(frequency);

    enabled = envelope.enabled();
    
    if (sweep.shift)
        updateSweep(false);

    updateTimer();
}

void Square1::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    constexpr auto kWaves = 0b00111111'00001111'00000011'00000001;

    sample = (kWaves >> (8 * pattern + step)) & 0x1;
    sample *= envelope.volume;

    step = (step + 1) % 8;

    updateTimer();
}

void Square1::tickSweep()
{
    if (!(enabled || sweep.tick()))
        return;

    updateSweep(true);
    updateSweep(false);

    sweep.init();
}

void Square1::tickLength()
{
    if (enabled)
    {
        length.tick();

        enabled = length.enabled();
    }
}

void Square1::tickEnvelope()
{
    if (enabled)
    {
        envelope.tick();

        enabled = envelope.enabled();
    }
}

void Square1::updateTimer()
{
    constexpr auto kWaveBits  = 8;
    constexpr auto kFrequency = 131072;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveBits;
}

void Square1::updateSweep(bool writeback)
{
    uint freq = sweep.next();
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
