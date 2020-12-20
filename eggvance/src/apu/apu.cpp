#include "apu.h"

#include "base/config.h"

Apu::Apu()
{
    if (config.bios_skip)
    {
        soundcnth.value = 0x880E;
        soundbias.value = 0x0200;
    }
}
