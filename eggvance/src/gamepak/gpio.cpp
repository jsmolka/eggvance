#include "gpio.h"

#include <shell/utility.h>

#include "base/bit.h"
#include "base/macros.h"

Gpio::Gpio()
    : type(Type::None)
{

}

Gpio::Gpio(Type type)
    : type(type)
{

}

void Gpio::reset()
{
    shell::reconstruct(*this, type);
}

bool Gpio::isReadable() const
{
    return readable;
}

u16 Gpio::read(u32 addr)
{
    switch (addr)
    {
    case kAddrData:
    {
        uint value = readPort() & maskGpioToGba();

        data &= maskGbaToGpio();
        data |= value;

        return value;
    }

    case kAddrDirection:
        return direction;

    case kAddrControl:
        return readable;
    }
    return 0;
}

void Gpio::write(u32 addr, u16 half)
{
    switch (addr)
    {
    case kAddrData:
        data &= maskGpioToGba();
        data |= maskGbaToGpio() & half;

        writePort(data);
        break;

    case kAddrDirection:
        direction = half & 0xF;
        break;

    case kAddrControl:
        readable = half & 0x1;
        break;
    }
}

u16 Gpio::readPort()
{
    return 0;
}

void Gpio::writePort(u16 half)
{
    
}

bool Gpio::isGpioToGba(uint port) const
{
    SHELL_ASSERT(port < 4);

    return maskGpioToGba() & (1 << port);
}

bool Gpio::isGbaToGpio(uint port) const
{
    SHELL_ASSERT(port < 4);

    return maskGbaToGpio() & (1 << port);
}

uint Gpio::maskGpioToGba() const
{
    return ~direction;
}

uint Gpio::maskGbaToGpio() const
{
    return direction;
}
