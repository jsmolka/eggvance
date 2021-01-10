#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"
#include "sweep.h"

class Square1 : public Channel<0x007F, 0xFFC0, 0x4000>
{
public:
    void tick();

    template<uint Index>
    void writeL(u8 byte)
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
    void writeH(u8 byte)
    {
        Channel::writeH<Index>(byte);

        if (Index == 0)
        {
            length.initial = bit::seq<0, 6>(byte);
            wave_duty      = bit::seq<6, 2>(byte);
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
                length.init();
                envelope.init();
            }
        }
    }

    Sweep sweep;
    Envelope envelope;
    Length length;
    uint step      = 0;
    uint wave_duty = 0;
    uint frequency = 0;
};
