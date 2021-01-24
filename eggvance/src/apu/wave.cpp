#include "wave.h"

#include "constants.h"

void Wave::init()
{
    if (two_banks)
        nibble = 0;
    else
        nibble = 16 * bank;

    length.init();

    // Todo: disable if zero volume
    enabled = true;

    updateTimer();
}

void Wave::tick()
{
    if (!(timer && --timer == 0 && active))
        return;

    // Todo: apply volume shift
    sample = bit::nibble(ram[nibble / 2], nibble & 0x1);

    if (two_banks)
        nibble = (nibble + 1) % 64;
    else
        nibble = (nibble + 1) % 32 + 16 * bank;

    updateTimer();
}

void Wave::tickLength()
{
    length.tick();

    enabled &= length.enabled();
}

u8 Wave::readBank(uint index) const
{
    return ram[16 * (bank ^ 0x1) + index];
}

void Wave::writeBank(uint index, u8 byte)
{
    ram[16 * (bank ^ 0x1) + index] = byte;
}

void Wave::updateTimer()
{
    constexpr auto kFrequency = 2097152;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency);
}
