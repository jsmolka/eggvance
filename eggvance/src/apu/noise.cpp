#include "noise.h"

#include "constants.h"

Noise::Noise()
    : Channel(0x0000'40FF'0000'FF00, 64)
{

}

void Noise::tick()
{
    if (!(enabled && timer && --timer == 0))
        return;

    sample = noise & 0x1;
    sample *= envelope.volume;

    noise >>= 1;
    if (sample)
        noise ^= 0x6000 >> narrow;

    timer = period();
}

void Noise::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    enum NR { k41 = 0, k42 = 1, k43 = 4, k44 = 5 };

    switch (index)
    {
    case NR::k41:
        length = seq<0, 6>();
        break;

    case NR::k42:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case NR::k43:
        ratio  = seq<32, 3>();
        narrow = seq<35, 1>() * 8;
        shift  = seq<36, 4>();
        break;

    case NR::k44:
        length.expire = seq<46, 1>();

        if (byte & 0x80)
            init();
        break;
    }
}

void Noise::init()
{
    Channel::init(true);

    initEnvelope();

    noise = 0x4000 >> narrow;
}

uint Noise::period() const
{
    constexpr auto kBaseFrequency = 524288;

    uint r = std::max<uint>(2 * ratio, 1);
    uint s = 1 << shift;

    return kCpuFrequency / (kBaseFrequency / r / s);
}
