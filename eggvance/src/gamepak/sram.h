#pragma once

#include "gamepak.h"

class Sram : public Save
{
public:
    Sram(const fs::path& file);

    virtual u8 readByte(u32 addr) override final;
    virtual void writeByte(u32 addr, u8 byte) override final;
};
