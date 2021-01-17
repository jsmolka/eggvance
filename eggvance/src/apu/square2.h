#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"

class Square2 : public Channel<0xFFC0, 0x0000, 0x4000>
{
public:
    Square2();

    void tick();

    template<uint Index>
    void writeL(u8 byte)
    {
        Channel::writeL<Index>(byte);

        if (Index == 0)
        {
            length.length = bit::seq<0, 6>(byte);
            wave_duty     = bit::seq<6, 2>(byte);
        }
        if (Index == 1)
        {
            envelope.write(byte);
        }
    }

    template<uint Index>
    void writeX(u8 byte)
    {
        Channel::writeX<Index>(byte);

        frequency = bit::seq<0, 11>(x.value);

        if (Index == 1)
        {
            length.expire = bit::seq<6, 1>(byte);

            if (byte & 0x80)
            {
            }
        }
    }

    Envelope envelope;
    Length length;
    uint step      = 0;
    uint wave_duty = 0;
    uint frequency = 0;
};
