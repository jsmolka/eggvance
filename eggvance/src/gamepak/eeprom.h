#pragma once

#include <vector>

#include "save.h"

class EEPROM : public Save
{
public:
    EEPROM(const std::string& file);

    virtual u8 readByte(u32);
    virtual void writeByte(u32, u8 byte) final;

private:
    enum State
    {
        SF_RECEIVE,
        SF_READ,
        SF_READ_NIBBLE,
        SF_READ_ADDRESS,
        SF_WRITE,   
        SF_WRITE_ADDRESS
    };

    int bus() const;
    void reset();

    State state;
    int addr;
    int buffer;
    int transmitted;
};
