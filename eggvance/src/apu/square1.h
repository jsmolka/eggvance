#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"
#include "sweep.h"

class Square1 : public Channel<0x007F, 0xFFFF, 0x47FF>
{
public:
    void init();
    void tick();
    void tickSweep();
    void tickLength();
    void tickEnvelope();

    template<uint Index> void writeL(u8 byte);
    template<uint Index> void writeH(u8 byte);
    template<uint Index> void writeX(u8 byte);

private:
    void updateTimer();
    void updateSweep(bool writeback);

    Length<64> length;
    Envelope envelope;
    Sweep sweep;

    uint step      = 0;
    uint pattern   = 0;
    uint frequency = 0;
};

template<uint Index>
void Square1::writeL(u8 byte)
{
    Channel::writeL<Index>(byte);

    if (Index == 0)
    {
        sweep.write(byte);
    }
}

template<uint Index>
void Square1::writeH(u8 byte)
{
    Channel::writeH<Index>(byte);

    if (Index == 0)
    {
        length  = bit::seq<0, 6>(byte);
        pattern = bit::seq<6, 2>(byte);
    }
    if (Index == 1)
    {
        envelope.write(byte);

        enabled &= envelope.enabled();
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
            init();
    }
}
