#pragma once

#include "gamepak/save.h"

class Sram : public Save
{
public:
    explicit Sram(const fs::path& file);

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

    static constexpr uint kSize = 0x8000;
};
