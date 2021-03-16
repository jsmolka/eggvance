#pragma once

#include "io.h"

class Sio
{
public:
    SioControl siocnt;
    RemoteControl rcnt;

    XRegister<u16> joycnt;
    XRegister<u32> joyrecv;
    XRegister<u32> joytrans;
    XRegister<u16> joystat;
    XRegister<u16> siomulti[4];
    XRegister<u16> siosend;
};

inline Sio sio;
