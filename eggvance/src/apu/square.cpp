#include "square.h"

#include "constants.h"

Square::Square(u64 mask)
    : Channel(mask, 64)
{

}

void Square::init()
{
    length.init();
    envelope.init();

    enabled = envelope.enabled();
    step = 0;
    timer = period();
}

void Square::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    constexpr auto kWaves = 0b00111111'00001111'00000011'00000001;

    sample = (kWaves >> (8 * pattern + step)) & 0x1;
    sample *= envelope.volume;

    step = (step + 1) % 8;

    timer = period();
}

uint Square::period() const
{
    constexpr auto kWaveBits  = 8;
    constexpr auto kFrequency = 131072;

    return (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveBits;
}
