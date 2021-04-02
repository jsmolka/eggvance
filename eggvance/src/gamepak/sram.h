#pragma once

#include "save.h"

class Sram : public Save
{
public:
    static constexpr auto kSize = 32 * 1024;

    Sram();

    void reset() final;
    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool valid(uint size) const final;
};
