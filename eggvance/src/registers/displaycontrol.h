#pragma once

#include "register.h"
#include "common/bits.h"
#include "common/config.h"
#include "common/macros.h"

class DisplayControl : public TRegister<DisplayControl, 2>
{
public:
    DisplayControl()
    {
        blank = config.bios_skip;
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            mode     = bits<0, 3>(byte);
            frame    = bits<4, 1>(byte) * 0xA000;
            oam_free = bits<5, 1>(byte);
            mapping  = bits<6, 1>(byte);
            blank    = bits<7, 1>(byte);
        }
        else
        {
            layers = bits<0, 5>(byte);
            win0   = bits<5, 1>(byte);
            win1   = bits<6, 1>(byte);
            winobj = bits<7, 1>(byte);
        }
    }

    inline bool isActive() const
    {
        switch (mode)
        {
        case 0: return 0b11111 & layers;
        case 1: return 0b10111 & layers;
        case 2: return 0b11100 & layers;
        case 3: return 0b10100 & layers;
        case 4: return 0b10100 & layers;
        case 5: return 0b10100 & layers;
        case 6: return false;
        case 7: return false;

        default:
            EGG_UNREACHABLE;
            return false;
        }
    }
    
    inline bool isBitmap() const
    {
        return mode > 2;
    }

    uint mode     = 0;
    uint frame    = 0;
    uint oam_free = 0;
    uint mapping  = 0;
    uint blank    = 0;
    uint layers   = 0;
    uint win0     = 0;
    uint win1     = 0;
    uint winobj   = 0;
};
