#pragma once

#include "common/config.h"
#include "register.h"

struct SerialIO
{
    SerialIO()
    {
        if (config.bios_skip)
        {
            rcnt.write<0>(0x00);
            rcnt.write<1>(0x80);
        }
    }

    RegisterRW<2> rcnt;
    RegisterRW<2> joycnt;
    RegisterRW<4> joyrecv;
    RegisterRW<4> joytrans;
    RegisterRW<2> joystat;

    RegisterRW<2> siocnt;
    RegisterRW<2> siodata8;
    RegisterRW<4> siodata32;
    RegisterRW<2> siomulti[4];
    RegisterRW<2> siosend;
};
