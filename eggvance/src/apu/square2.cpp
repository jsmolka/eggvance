#include "square2.h"

#include "constants.h"

Square2::Square2()
{
    mask = 0x0000'4000'0000'FFC0;
}

void Square2::init()
{
    length.init();
    envelope.init();

    enabled = envelope.enabled();

    updateTimer();
}

void Square2::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    constexpr auto kWaves = 0b00111111'00001111'00000011'00000001;

    sample = (kWaves >> (8 * pattern + step)) & 0x1;
    sample *= envelope.volume;

    step = (step + 1) % 8;

    updateTimer();
}

void Square2::tickLength()
{
    if (enabled)
    {
        length.tick();

        enabled = length.enabled();
    }
}

void Square2::tickEnvelope()
{
    if (enabled)
    {
        envelope.tick();

        enabled = envelope.enabled();
    }
}

void Square2::write(std::size_t index, u8 byte)
{
    Channel::write(index, byte);

    switch (index)
    {
    case NR::k21:
        length  = seq<0, 6>();
        pattern = seq<6, 2>();
        break;

    case NR::k22:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case NR::k23:
        frequency = seq<32, 11>();
        break;

    case NR::k24:
        frequency     = seq<32, 11>();
        length.expire = seq<46,  1>();

        if (byte & 0x80)
            init();
        break;
    }
}

void Square2::updateTimer()
{
    constexpr auto kWaveBits  = 8;
    constexpr auto kFrequency = 131072;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveBits;
}
