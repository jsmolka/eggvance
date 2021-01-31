#include "wave.h"

#include "constants.h"

Wave::Wave()
    : Channel(0x0000'4000'E000'00E0, 256)
{

}

void Wave::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    sample = bit::nibble(ram[step >> 1], (step ^ 0x1) & 0x1);
    sample = volume * sample / 4;

    if (wide)
        step = (step + 1) % 64;
    else
        step = (step + 1) % 32 + 16 * ram.bank;

    timer = period();
}

void Wave::write(uint index, u8 byte)
{
    static constexpr uint kVolumes[8] = { 0, 4, 2, 1, 3, 3, 3, 3 };

    Channel::write(index, byte);

    switch (index)
    {
    case 0:
        wide     = seq<5, 1>();
        ram.bank = seq<6, 1>();
        active   = seq<7, 1>();
        enabled &= active;
        break;

    case 2:
        length = byte;
        break;

    case 3:
        volume = kVolumes[seq<29, 3>()];
        break;

    case 4:
        frequency = seq<32, 11>();
        break;

    case 5:
        frequency     = seq<32, 11>();
        length.expire = seq<46,  1>();

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
