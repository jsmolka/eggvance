#pragma once

#include "common/config.h"
#include "registers/register.h"

struct MiscIO
{
    MiscIO()
    {
        postflag.write<0>(config.bios_skip);

        if (config.bios_skip)
        {
            memory_control.write<0>(0x20);
            memory_control.write<1>(0x00);
            memory_control.write<2>(0x00);
            memory_control.write<3>(0xD0);
        }
    }

    RegisterRW<1> postflag;
    RegisterRW<2> greenswap;
    RegisterRW<4> memory_control;
};
