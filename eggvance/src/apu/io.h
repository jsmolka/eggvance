#pragma once

#include "base/register.h"

class SoundControl : public Register<u64, 0x0000'0080'770F'FF77>
{
public:
    SoundControl();

    u8 read(uint index) const;
    void write(uint index, u8 byte);

    uint volume    = 0;
    uint volume_r  = 0;
    uint volume_l  = 0;
    uint enabled   = 0;
    uint enabled_r = 0;
    uint enabled_l = 0;
};

class SoundBias : public Register<u16, 0xC3FF>
{
public:
    SoundBias();

    operator uint() const;
};
