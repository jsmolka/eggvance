#pragma once

#include "base/bit.h"
#include "base/int.h"

class Wave
{
public:
    template<uint Index>
    u8 readL()  // NR30
    {
        if (Index == 0)
        {
            return dimension << 5
                 | bank << 6
                 | enable << 7;
        }
        return 0;
    }

    template<uint Index>
    void writeL(u8 byte)  // NR30
    {
        if (Index == 0)
        {
            dimension = bit::seq<5, 1>(byte);
            bank = bit::seq<6, 1>(byte);
            enable = bit::seq<7, 1>(byte);
        }
    }

    template<uint Index>
    u8 readH()  // NR31, NR32
    {
        if (Index == 0)
        {
            return 0;
        }
        else
        {
            return volume << 5
                | forced << 7;
        }

    }

    template<uint Index>
    void writeH(u8 byte)  // NR31, NR32
    {
        if (Index == 0)
        {
            length = byte;
        }
        else
        {
            volume = bit::seq<5, 2>(byte);
            forced = bit::seq<7, 1>(byte);
        }
    }

    template<uint Index>
    u8 readX()  // NR33, NR34
    {
        if (Index == 0)
        {
            return 0;
        }
        else
        {
            return expire << 6;
        }
    }

    template<uint Index>
    void writeX(u8 byte)  // NR33, NR34
    {
        if (Index == 0)
        {
            sample_rate = (sample_rate & ~0xFF) | byte;
        }
        else if (Index == 1)
        {
            sample_rate = (sample_rate & ~0x700) | bit::seq<0, 3>(byte) << 8;
            expire = bit::seq<6, 1>(byte);

            if (byte & 0x80)
            {
                // Restart
            }
        }
    }

private:
    // L
    uint dimension = 0;
    uint bank = 0;
    uint enable = 0;
    // H
    uint length = 0;
    uint volume = 0;
    uint forced = 0;  // Force volume to 75%
    // X
    uint sample_rate = 0;  // 2097152/(2048-n) Hz
    uint expire = 0;
    uint restart = 0;
};
