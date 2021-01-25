#pragma once

#include <algorithm>
#include <cmath>

#include "channel.h"
#include "envelope.h"
#include "length.h"

class Noise : public Channel<0xFF3F, 0x0000, 0x40FF>
{
public:
    void init();
    void tick();
    void tickLength();
    void tickEnvelope();

    template<uint Index> void writeL(u8 byte);
    template<uint Index> void writeX(u8 byte);

private:
    void updateTimer();

    Length<64> length;
    Envelope envelope;

    uint shift     = 0;
    uint frequency = 1;
    uint noise     = 0;
};

template<uint Index>
void Noise::writeL(u8 byte)
{
    Channel::writeL<Index>(byte);

    if (Index == 0)
    {
        length.length = bit::seq<0, 6>(byte);
    }
    if (Index == 1)
    {
        envelope.write(byte);

        enabled &= envelope.enabled();
    }
}

template<uint Index>
void Noise::writeX(u8 byte)
{
    Channel::writeX<Index>(byte);

    if (Index == 0)
    {
        double r = bit::seq<0, 3>(byte);
        shift    = bit::seq<3, 1>(byte) * 8;
        double s = bit::seq<4, 4>(byte);

        frequency = static_cast<uint>(524288.0 / std::max(r, 0.5) / std::pow(2, s + 1));
    }
    if (Index == 1)
    {
        length.expire = bit::seq<6, 1>(byte);

        if (byte & 0x80)
            init();
    }
}
