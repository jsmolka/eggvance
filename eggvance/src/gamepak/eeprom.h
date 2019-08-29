#pragma once

#include "save.h"

class EEPROM : public Save
{
public:
    EEPROM(const std::string& file);

    virtual u8 readByte(u32) final;
    virtual void writeByte(u32, u8 byte) final;

private:
    enum State
    {
        STATE_RECEIVE,
        STATE_READ,
        STATE_READ_NIBBLE,
        STATE_READ_ADDRESS,
        STATE_WRITE,   
        STATE_WRITE_ADDRESS
    };

    void resetBuffer();
    int bus() const;

    State state;
    int addr;
    int buffer;
    int transmitted;
};
