#include "sio.h"

#include "base/config.h"

Sio::Sio()
{
    if (config.bios_skip)
    {
        rcnt.value = 0x8000;
    }
}
