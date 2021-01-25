#include "wave.h"

#include "constants.h"

void Wave::init()
{
    if (dimension)
        position = 0;
    else
        position = 16 * bank;

    length.init();

    enabled = active;

    updateTimer();
}

void Wave::tick()
{
    if (!(timer && --timer == 0))
        return;

    sample = bit::nibble(ram[position / 2], (position & 0x1) ^ 0x1);
    sample = volume * sample / 4;

    if (dimension)
        position = (position + 1) % 64;
    else
        position = (position + 1) % 32 + 16 * bank;

    updateTimer();
}

void Wave::tickLength()
{
    length.tick();

    enabled &= length.enabled();
}

u8 Wave::readRam(uint index) const
{
    return ram[16 * (bank ^ 0x1) + index];
}

void Wave::writeRam(uint index, u8 byte)
{
    ram[16 * (bank ^ 0x1) + index] = byte;
}

void Wave::updateTimer()
{
    constexpr auto kFrequency = 2097152;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency);
}
