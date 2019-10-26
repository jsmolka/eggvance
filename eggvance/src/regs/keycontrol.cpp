#include "keycontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void KeyControl::reset()
{
    *this = {};
}

u8 KeyControl::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void KeyControl::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        bcast(mask)[0] = byte;
    }
    else
    {
        bcast(mask)[1] = bits<0, 2>(byte);
        irq            = bits<6, 1>(byte);
        logic          = bits<7, 1>(byte);
    }
    data[index] = byte;
}
