#pragma once

#include "save.h"

class EEPROM : public Save
{
public:
    EEPROM(const std::string& file);
    ~EEPROM();

    virtual u8 readByte(u32 addr);
    virtual void writeByte(u32 addr, u8 byte) final;
};
