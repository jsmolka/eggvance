#include "wave.h"

#include "constants.h"

Wave::Wave()
{
    mask = 0x0000'4000'E000'00E0;
}

void Wave::init()
{
    if (dimension)
        position = 0;
    else
        position = 16 * ram.bank;

    length.init();

    enabled = active;

    updateTimer();
}

void Wave::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    sample = bit::nibble(ram[position / 2], (position & 0x1) ^ 0x1);
    sample = volume * sample / 4;

    if (dimension)
        position = (position + 1) % 64;
    else
        position = (position + 1) % 32 + 16 * ram.bank;

    updateTimer();
}

void Wave::tickLength()
{
    if (enabled)
    {
        length.tick();

        enabled = length.enabled();
    }
}

void Wave::write(std::size_t index, u8 byte)
{
    static constexpr uint kVolumes[8] = { 0, 4, 2, 1, 3, 3, 3, 3 };

    Channel::write(index, byte);

    switch (index)
    {
    case NR::k30:
        dimension = seq<5, 1>();
        ram.bank  = seq<6, 1>();
        active    = seq<7, 1>();
        enabled &= active;
        break;

    case NR::k31:
        length = byte;
        break;

    case NR::k32:
        volume = kVolumes[seq<29, 3>()];
        break;

    case NR::k33:
        frequency = seq<32, 11>();
        break;

    case NR::k34:
        frequency     = seq<32, 11>();
        length.expire = seq<46,  1>();

        if (byte & 0x80)
            init();
        break;
    }
}

void Wave::updateTimer()
{
    constexpr auto kFrequency = 2097152;

    timer = (kCpuFrequency / kFrequency) * (2048 - frequency);
}
