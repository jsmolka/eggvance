#pragma once

#include "save.h"

class Sram final : public Save
{
public:
    Sram();

    void reset() final;
    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool isValidSize(uint size) const final;
};
