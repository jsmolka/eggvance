#pragma once

#include <vector>

#include "save.h"

class SRAM : public Save
{
public:
    SRAM(const std::string& file);
    ~SRAM();

    virtual u8 readByte(u32 addr) final;
    virtual void writeByte(u32 addr, u8 byte) final;
    
private:
    std::vector<u8> data;
};
