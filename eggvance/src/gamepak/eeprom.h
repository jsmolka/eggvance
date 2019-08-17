#pragma once

#include <vector>

#include "save.h"

class EEPROM : public Save
{
public:
    EEPROM(const std::string& file, std::size_t rom_size);
    ~EEPROM();

    virtual u8 readByte(u32);
    virtual void writeByte(u32, u8 byte) final;

private:
    enum State
    {
        SF_RECEIVE       = 1 << 1,
        SF_READ          = 1 << 2,
        SF_READ_NIBBLE   = 1 << 3,
        SF_READ_ADDRESS  = 1 << 4,
        SF_WRITE         = 1 << 5,   
        SF_WRITE_ADDRESS = 1 << 6
    };

    void reset();

    State state;

    int bus;
    int addr;
    int buffer;
    int transmitted;

    std::vector<u8> data;
};
