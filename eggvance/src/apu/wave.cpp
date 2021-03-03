#include "wave.h"

#include "base/constants.h"

Wave::Wave()
    : Channel(0x0000'4000'E000'00E0, 256)
{

}

void Wave::tick()
{
    uint ticks = run();
    if (!ticks)
        return;

    setStep(step + ticks);

    sample = volume * ram[step] / 4;
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
        frequency = bit::seq<32, 11>(value);
        break;

    case 5:
        frequency     = bit::seq<32, 11>(value);
        length.expire = bit::seq< 6,  1>(byte);

        if (byte & 0x80)
            init();
        break;
    }
}

uint Wave::period() const
{
    constexpr auto kFrequency = 2097152;

    return (kCpuFrequency / kFrequency) * (2048 - frequency);
}

void Wave::init()
{
    Channel::init(active);

    setStep(0);
}

void Wave::setStep(uint value)
{
    if (wide)
        step = value % 64;
    else
        step = value % 32 + 32 * ram.bank;
}
