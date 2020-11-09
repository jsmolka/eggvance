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

    Gpio();
    explicit Gpio(Type type);
    virtual ~Gpio() = default;

    virtual void reset();

    bool isReadable() const;
    bool isAccess(u32 addr) const;

    u16 read(u32 addr);
    void write(u32 addr, u16 half);

    const Type type;

protected:
    virtual u16 readPort();
    virtual void writePort(u16 half);

    bool isGpioToGba(uint port) const;
    bool isGbaToGpio(uint port) const;

private:
    enum Address
    {
        kAddrData      = 0xC4,
        kAddrDirection = 0xC6,
        kAddrControl   = 0xC8
    };

    uint maskGpioToGba() const;
    uint maskGbaToGpio() const;

    uint data      = 0;
    uint direction = 0;
    uint readable  = 0;
};
