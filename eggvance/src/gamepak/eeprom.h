#pragma once

#include "gamepak/save.h"

class Eeprom : public Save
{
public:
    explicit Eeprom(const fs::path& file);

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

private:
    enum class State
    {
        Receive,
        ReadSetAddress,
        ReadSetAddressEnd,
        ReadUnused,
        Read,
        WriteSetAddress,
        Write,
        WriteEnd
    } state;

    void setState(State state);

    uint count = 0;
    uint buffer = 0;
    uint address = 0;
};
