#pragma once

#include "base/int.h"

class Gpio
{
public:
    enum class Type
    {
        None,
        Rtc
    };

    enum Port
    {
        kPortData       = 0x0800'00C4,
        kPortDirection  = 0x0800'00C6,
        kPortReadEnable = 0x0800'00C8
    };

    Gpio();
    explicit Gpio(Type type);
    virtual ~Gpio() = default;

    virtual void reset();

    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Type type;
    uint readable;

protected:
    virtual u16 readPort();
    virtual void writePort(u16 half);

private:
    uint maskGpioToGba() const;
    uint maskGbaToGpio() const;

    uint direction;
    uint data;
};
