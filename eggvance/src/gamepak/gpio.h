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

    enum Address
    {
        kAddressData      = 0x0800'00C4,
        kAddressDirection = 0x0800'00C6,
        kAddressReadable  = 0x0800'00C8
    };

    Gpio();
    explicit Gpio(Type type);
    virtual ~Gpio() = default;

    virtual void reset();

    bool isReadable() const;

    u8 read(u32 addr);
    void write(u32 addr, u8 byte);

    const Type type;

protected:
    virtual u8 readPort();
    virtual void writePort(u8 byte);

    bool isGpioToGba(uint port) const;
    bool isGbaToGpio(uint port) const;

private:
    uint maskGpioToGba() const;
    uint maskGbaToGpio() const;

    uint data      = 0;
    uint direction = 0;
    uint readable  = 0;
};
