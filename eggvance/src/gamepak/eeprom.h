#pragma once

#include "save.h"

class Eeprom : public Save
{
public:
    Eeprom();

    static constexpr uint kSize4  = 0x0200;
    static constexpr uint kSize64 = 0x2000;

    void reset() final;

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool isValidSize() const final;

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

    uint bus() const;
    void setState(State state);

    uint count   = 0;
    uint buffer  = 0;
    uint address = 0;
};
