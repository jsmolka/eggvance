#include "noise.h"

#include <algorithm>
#include <cmath>

#include "constants.h"

Noise::Noise()
    : Channel(0x0000'40FF'0000'FF00, 64)
{
    frequency = 1;
}

void Noise::init()
{
    noise = 0x4000 >> shift;

    length.init();
    envelope.init();

    enabled = envelope.enabled();

    timer = period();
}

void Noise::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    sample = noise & 0x1;
    sample *= envelope.volume;

    noise >>= 1;
    if (sample)
        noise ^= 0x6000 >> shift;

    timer = period();
}

uint Noise::period() const
{
    return kCpuFrequency / frequency;
}

void Noise::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    double r;
    double s;

    switch (index)
    {
    case NR::k41:
        length.length = seq<0, 6>();
        break;

    case NR::k42:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case NR::k43:
        r         = seq<32, 3>();
        shift     = seq<35, 1>() * 8;
        s         = seq<36, 4>();
        frequency = static_cast<uint>(524288.0 / std::max(r, 0.5) / std::pow(2, s + 1));
        break;

    case NR::k44:
        length.expire = seq<46,  1>();

        if (byte & 0x80)
            init();
        break;
    }
}
