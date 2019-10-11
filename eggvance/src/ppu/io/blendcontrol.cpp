#include "blendcontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void BlendControl::Layer::reset()
{
    bg0   = 0;
    bg1   = 0;
    bg2   = 0;
    bg3   = 0;
    obj   = 0;
    bdp   = 0;
    flags = 0;
}

u8 BlendControl::Layer::readByte()
{
    return flags;
}

void BlendControl::Layer::writeByte(u8 byte)
{
    bg0   = bits<0, 1>(byte);
    bg1   = bits<1, 1>(byte);
    bg2   = bits<2, 1>(byte);
    bg3   = bits<3, 1>(byte);
    obj   = bits<4, 1>(byte);
    bdp   = bits<5, 1>(byte);
    flags = byte & 0x3F;
}

void BlendControl::reset()
{
    mode = 0;
    upper.reset();
    lower.reset();
}

u8 BlendControl::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= upper.readByte();
        byte |= mode << 6;
        break;

    case 1:
        byte |= lower.readByte();
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

void BlendControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0:
        upper.writeByte(byte);
        mode = bits<6, 2>(byte);
        break;

    case 1:
        lower.writeByte(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
