#pragma once

#include "save.h"
#include "serialbuffer.h"

class Eeprom : public Save
{
public:
    static constexpr auto kSize4KBit  = 0x0200;
    static constexpr auto kSize64KBit = 0x2000;

    Eeprom();

    void reset() final;
    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool valid(uint size) const final;

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
    } state = State::Receive;

    uint bus() const;
    void setState(State state);

    uint address = 0;
    SerialBuffer<u64> buffer;
};
