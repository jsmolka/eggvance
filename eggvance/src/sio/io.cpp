#include "io.h"

#include "arm/arm.h"
#include "arm/constants.h"
#include "base/config.h"

RemoteControl::RemoteControl()
{
    if (config.bios_skip)
    {
        write(0, 0x00);
        write(1, 0x80);
    }
}

void SioControl::write(uint index, u8 byte)
{
    if (!XRegister::write(index, byte))
        return;

    if (index == 0)
    {
        constexpr auto kEnabled = 1 << 7;

        if (value & kEnabled)
        {
            if (irq)
                arm.raise(kIrqSerial);

            value &= ~kEnabled;
        }
    }
    else
    {
        irq = bit::seq<6, 1>(byte);
    }
}
