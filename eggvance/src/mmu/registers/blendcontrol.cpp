#include "blendcontrol.h"

#include "common/utility.h"

void BlendControl::Layer::write(u8 byte)
{
    bg0 = bits<0, 1>(byte);
    bg1 = bits<1, 1>(byte);
    bg2 = bits<2, 1>(byte);
    bg3 = bits<3, 1>(byte);
    obj = bits<4, 1>(byte);
    bdp = bits<5, 1>(byte);
}

void BlendControl::write(int index, u8 byte)
{
    switch (index)
    {
    case 0:
        upper.write(byte);
        mode = bits<6, 2>(byte);
        break;

    case 1:
        lower.write(byte);
        break;
    }
}
