#include "square.h"

#include "constants.h"

Square::Square(u64 mask)
    : Channel(mask, 64)
{

}

void Square::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    constexpr u8 kWaves[4] = {
        0b00000001,
        0b00000011,
        0b00001111,
        0b00111111
    };

    sample = (kWaves[form] >> step) & 0x1;
    sample *= envelope.volume;

    step = (step + 1) % 8;

    timer = period();
}

void Square::init()
{
    Channel::init(true);

    initEnvelope();
}

uint Square::period() const
{
    constexpr auto kWaveBits  = 8;
    constexpr auto kFrequency = 131072;

    return (kCpuFrequency / kFrequency) * (2048 - frequency) / kWaveBits;
}
