#include "displaycontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void DisplayControl::reset()
{
    mode        = 0;
    frame       = 0;
    oam_hblank  = 0;
    mapping_1d  = 0;
    force_blank = 0;
    bg0         = 0;
    bg1         = 0;
    bg2         = 0;
    bg3         = 0;
    obj         = 0;
    win0        = 0;
    win1        = 0;
    winobj      = 0;
}

u8 DisplayControl::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= mode        << 0;
        byte |= frame       << 4;
        byte |= oam_hblank  << 5;
        byte |= mapping_1d  << 6;
        byte |= force_blank << 7;
        break;

    case 1:
        byte |= bg0    << 0;
        byte |= bg1    << 1;
        byte |= bg2    << 2;
        byte |= bg3    << 3;
        byte |= obj    << 4;
        byte |= win0   << 5;
        byte |= win1   << 6;
        byte |= winobj << 7;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

void DisplayControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0:
        mode        = bits<0, 3>(byte);
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

    default:
        EGG_UNREACHABLE;
        break;
    }
}

bool DisplayControl::enabled() const
{
    switch (mode)
    {
    case 0: return bg0 || bg1 || bg2 || bg3 || obj;
    case 1: return bg0 || bg1 || bg2 || obj;
    case 2: return bg2 || bg3 || obj;
    case 3: return bg2 || obj;
    case 4: return bg2 || obj;
    case 5: return bg2 || obj; 
    }
    return false;
}
