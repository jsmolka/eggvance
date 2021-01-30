#include "square1.h"

#include "constants.h"

Square1::Square1()
{
    mask = 0x0000'4000'FFC0'007F;
}

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
    if (!(enabled && sweep.tick()))
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

void Square1::write(std::size_t index, u8 byte)
{
    Channel::write(index, byte);

    switch (index)
    {
    case NR::k10:
        sweep.write(byte);
        break;

    case NR::k11:
        length  = seq<16, 6>();
        pattern = seq<22, 2>();
        break;

    case NR::k12:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case NR::k13:
        frequency = seq<32, 11>();
        break;

    case NR::k14:
        frequency     = seq<32, 11>();
        length.expire = seq<48,  1>();

        if (byte & 0x80)
            init();
        break;
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
