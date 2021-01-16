#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"
#include "sweep.h"

class Square1 : public Channel<0x007F, 0xFFFF, 0xC7FF>
{
public:
    static constexpr auto kFrequency = 131072;

    void tick();

    template<uint Index> void writeL(u8 byte);
    template<uint Index> void writeH(u8 byte);
    template<uint Index> void writeX(u8 byte);

    Sweep sweep;
    Envelope envelope;
    Length length;
    uint timer     = 0;
    uint step      = 0;
    uint pattern = 0;
    uint frequency = 0;

    s16 sample = 0;

private:
    void updateTimer();
};

template<uint Index>
void Square1::writeL(u8 byte)
{
    Channel::writeL<Index>(byte);

    if (Index == 0)
    {
        sweep.shift     = bit::seq<0, 3>(byte);
        sweep.direction = bit::seq<3, 1>(byte);
        sweep.time      = bit::seq<4, 3>(byte);
    }
}

template<uint Index>
void Square1::writeH(u8 byte)
{
    Channel::writeH<Index>(byte);

    if (Index == 0)
    {
        length.initial = bit::seq<0, 6>(byte);
        pattern      = bit::seq<6, 2>(byte);
    }
    if (Index == 1)
    {
        envelope.write(byte);
    }
}

template<uint Index>
void Square1::writeX(u8 byte)
{
    Channel::writeX<Index>(byte);

    frequency = bit::seq<0, 11>(x.value);

    if (Index == 1)
    {
        length.expire = bit::seq<6, 1>(byte);

        if (byte & 0x80)
        {
            length.init();
            envelope.init();
            updateTimer();
        }
    }
}
