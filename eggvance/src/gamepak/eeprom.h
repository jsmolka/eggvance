#pragma once

#include "save.h"
#include "serialbuffer.h"

class Eeprom final : public Save
{
public:
    Eeprom();

    void initBus6();
    void initBus14();
    bool isInitialized() const;

    void reset() final;
    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

protected:
    bool isValidSize(uint size) const final;

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
