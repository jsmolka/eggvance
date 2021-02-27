#include "noise.h"

#include <algorithm>

#include "base/constants.h"

Noise::Noise()
    : Channel(0x0000'40FF'0000'FF00, 64)
{

}

void Noise::tick()
{
    uint ticks = run();
    if (!ticks)
        return;

    while (ticks--)
    {
        sample = noise & 0x1;

        noise >>= 1;
        if (sample)
            noise ^= 0x6000 >> narrow;
    }

    sample *= envelope.volume;
}

void Noise::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    switch (index)
    {
    case 0:
        length = bit::seq<0, 6>(byte);
        break;

    case 1:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case 4:
        ratio  = bit::seq<0, 3>(byte);
        narrow = bit::seq<3, 1>(byte) * 8;
        shift  = bit::seq<4, 4>(byte);
        break;

    case 5:
        length.expire = bit::seq<6, 1>(byte);

        if (byte & 0x80)
            init();
        break;
    }
}

uint Noise::period() const
{
    constexpr auto kFrequency = 524288;

    uint r = std::max<uint>(2 * ratio, 1);
    uint s = 1 << shift;

    return kCpuFrequency / (kFrequency / r / s);
}

void Noise::init()
{
    Channel::init(true);

    initEnvelope();

    noise = 0x4000 >> narrow;
}
