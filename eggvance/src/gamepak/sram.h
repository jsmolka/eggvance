#pragma once

#include "save.h"

class Sram : public Save
{
public:
    static constexpr uint kSize = 0x8000;

    Sram();

    void reset() final;

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool isValid(uint size) const final;
};
