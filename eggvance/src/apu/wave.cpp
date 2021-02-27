#include "wave.h"

#include "constants.h"

Wave::Wave()
    : Channel(0x0000'4000'E000'00E0, 256)
{

}

void Wave::tick()
{
    if (wide)
        step = (step + run()) % 64;
    else
        step = (step + run()) % 32 + 16 * ram.bank;

    sample = bit::nibble(ram[step >> 1], step & 0x1 ^ 0x1);
    sample = volume * sample / 4;
}

void Wave::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    static constexpr uint kVolumes[8] = { 0, 4, 2, 1, 3, 3, 3, 3 };

    switch (index)
    {
    case 0:
        wide     = bit::seq<5, 1>(byte);
        ram.bank = bit::seq<6, 1>(byte);
        active   = bit::seq<7, 1>(byte);
        enabled &= active;
        break;

    case 2:
        length = byte;
        break;

    case 3:
        volume = kVolumes[bit::seq<5, 3>(byte)];
        break;

    case 4:
        frequency = bit::seq<32, 11>(data);
        break;

    case 5:
        frequency     = bit::seq<32, 11>(data);
        length.expire = bit::seq< 6,  1>(byte);

        if (byte & 0x80)
            init();
        break;
    }
}

void Wave::init()
{
    Channel::init(active);

    if (wide)
        step = 0;
    else
        step = 16 * ram.bank;
}

uint Wave::period() const
{
    constexpr auto kFrequency = 2097152;

    return (kCpuFrequency / kFrequency) * (2048 - frequency);
}
