#include "blendalpha.h"

#include "common/utility.h"

void BlendAlpha::write(int index, u8 byte)
{
    switch (index)
    {
    case 0: eva = bits<0, 5>(byte); break;
    case 1: evb = bits<0, 5>(byte); break;
    }
}
