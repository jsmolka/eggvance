#include "displaycontrol.h"

#include "common/utility.h"

void DisplayControl::write(int index, u8 byte)
{
    switch (index)
    {
    case 0:
        mode        = bits<0, 3>(byte);
        gbc         = bits<3, 1>(byte);
        frame       = bits<4, 1>(byte);
        oam_hblank  = bits<5, 1>(byte);
        mapping_1d  = bits<6, 1>(byte);
        force_blank = bits<7, 1>(byte);
        break;

    case 1:
        bg0    = bits<0, 1>(byte);
        bg1    = bits<1, 1>(byte);
        bg2    = bits<2, 1>(byte);
        bg3    = bits<3, 1>(byte);
        obj    = bits<4, 1>(byte);
        win0   = bits<5, 1>(byte);
        win1   = bits<6, 1>(byte);
        winobj = bits<7, 1>(byte);
        break;
    }
}
