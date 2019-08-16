#pragma once

#include "save.h"

class Flash : public Save
{
public:
    Flash(const std::string& file, Save::Type type);
    ~Flash();

    virtual u8 readByte(u32 addr) final;
    virtual void writeByte(u32 addr, u8 byte) final;
};
