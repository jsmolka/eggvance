#pragma once

#include "gamepak/save.h"

class Eeprom : public Save
{
public:
    Eeprom(const fs::path& file);

    virtual u8 readByte(u32 addr) override final;
    virtual void writeByte(u32, u8 byte) override final;

private:
    enum class State
    {
        Receive,
        Read,
        ReadNibble,
        ReadAddress,
        Write,   
        WriteAddress
    } state;

    void resetBuffer();

    int bus() const;

    int buffer;
    int address;
    int transmitted;
};
