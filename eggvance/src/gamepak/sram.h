#pragma once

#include "save.h"

class Sram : public Save
{
public:
    Sram();

    static constexpr uint kSize = 0x8000;

    void reset() final;

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool isValidSize() const final;
};
