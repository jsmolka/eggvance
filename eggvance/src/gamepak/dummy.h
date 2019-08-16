#pragma once

#include "save.h"

class Dummy : public Save
{
public:
    Dummy();

    virtual u8 readByte(u32 addr) final;
    virtual void writeByte(u32 addr, u8 byte) final;
};
