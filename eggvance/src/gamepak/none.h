#pragma once

#include "save.h"

class None : public Save
{
public:
    None();

    virtual u8 readByte(u32 addr) final;
    virtual void writeByte(u32 addr, u8 byte) final;
};
