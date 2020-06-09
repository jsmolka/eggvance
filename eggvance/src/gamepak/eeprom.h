#pragma once

#include "gamepak/save.h"

class Eeprom : public Save
{
public:
    explicit Eeprom(const fs::path& file);

    u8 read(u32 addr) final;
    void write(u32, u8 byte) final;

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
