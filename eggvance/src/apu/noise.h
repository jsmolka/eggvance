#pragma once

#include "channel.h"
#include "length.h"
#include "envelope.h"

class Noise : public Channel<0xFF00, 0x0000, 0x40FF>
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

    uint timer   = 0;
    uint divisor = 0;
    uint small   = 0;
    uint shift   = 0;
    uint lfsr    = 0;
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
        constexpr uint kDivisors[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };

        divisor = bit::seq<0, 3>(byte);
        small   = bit::seq<3, 1>(byte);
        shift   = bit::seq<4, 4>(byte);
    }
    if (Index == 1)
    {
        length.expire = bit::seq<6, 1>(byte);

        if (byte & 0x80)
            init();
    }
}
