#include "io.h"

#include "arm/arm.h"
#include "arm/constants.h"

void SioControl::write(uint index, u8 byte)
{
    XRegister::write(index, byte);

    if (index == 0)
    {
        uint was_enabled = enabled;

        enabled = bit::seq<7, 1>(byte);

        if (!was_enabled && enabled)
        {
            if (irq)
                arm.raise(kIrqSerial);

            enabled = false;
            value &= ~(1 << 7);
        }
    }
    else
    {
        irq = bit::seq<6, 1>(byte);
    }
}
