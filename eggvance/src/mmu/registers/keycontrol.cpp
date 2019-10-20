#include "keycontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void KeyControl::reset()
{
    keys      = 0;
    irq       = 0;
    irq_logic = 0;
}

void KeyControl::write(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0:
        byteArray(keys)[0] = byte;
        break;

    case 1:
        byteArray(keys)[1] = bits<0, 2>(byte);
        irq                = bits<6, 1>(byte);
        irq_logic          = bits<7, 1>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
