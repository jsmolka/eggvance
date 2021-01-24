#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"

class Square2 : public Channel<0xFFFF, 0x0000, 0x47FF>
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

    uint timer     = 0;
    uint step      = 0;
    uint pattern   = 0;
    uint frequency = 0;
};

template<uint Index>
void Square2::writeL(u8 byte)
{
    Channel::writeL<Index>(byte);

    if (Index == 0)
    {
        length.length = bit::seq<0, 6>(byte);
        pattern       = bit::seq<6, 2>(byte);
    }
    if (Index == 1)
    {
        envelope.write(byte);

        enabled &= envelope.enabled();
    }
}

template<uint Index>
void Square2::writeX(u8 byte)
{
    Channel::writeX<Index>(byte);

    frequency = bit::seq<0, 11>(x.value);

    if (Index == 1)
    {
        length.expire = bit::seq<6, 1>(byte);

        if (byte & 0x80)
            init();
    }
}
