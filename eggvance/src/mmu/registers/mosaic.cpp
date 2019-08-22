#include "mosaic.h"

#include "common/utility.h"

void Mosaic::write(int index, u8 byte)
{
    switch (index)
    {
    case 0:
        bg.write(byte);
        break;

    case 1:
        obj.write(byte);
        break;
    }
}

void Mosaic::Stretch::write(u8 byte)
{
    x  = bits<0, 4>(byte);
    y  = bits<4, 4>(byte);
}
