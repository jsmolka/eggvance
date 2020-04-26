#pragma once

#include "backup.h"

class SRAM : public Backup
{
public:
    SRAM(const fs::path& file);

    virtual u8 readByte(u32 addr) override final;
    virtual void writeByte(u32 addr, u8 byte) override final;
};
