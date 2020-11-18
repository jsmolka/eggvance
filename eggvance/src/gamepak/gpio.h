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

    enum Register
    {
        kRegData      = 0xC4,
        kRegDirection = 0xC6,
        kRegControl   = 0xC8
    };

    Gpio();
    explicit Gpio(Type type);
    virtual ~Gpio() = default;

    virtual void reset();

    bool isReadable() const;

    u16 read(u32 addr);
    void write(u32 addr, u16 half);

    const Type type;

protected:
    virtual u16 readPort();
    virtual void writePort(u16 half);

    bool isGpioToGba(uint port) const;
    bool isGbaToGpio(uint port) const;

private:
    u16 maskGpioToGba() const;
    u16 maskGbaToGpio() const;

    u16 data      = 0;
    u16 direction = 0;
    u16 readable  = 0;
};
