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
    u16 value = 0;

    switch (addr)
    {
    case kRegData:
        value = readPort() & maskGpioToGba();

        data &= maskGbaToGpio();
        data |= value;
        break;

    case kRegDirection:
        value = direction;
        break;

    case kRegControl:
        value = readable;
        break;
    }
    return value;
}

void Gpio::write(u32 addr, u16 half)
{
    switch (addr)
    {
    case kRegData:
        data &= maskGpioToGba();
        data |= maskGbaToGpio() & half;

        writePort(data);
        break;

    case kRegDirection:
        direction = bit::seq<0, 4>(half);
        break;

    case kRegControl:
        readable = bit::seq<0, 1>(half);
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

u16 Gpio::maskGpioToGba() const
{
    return ~direction;
}

u16 Gpio::maskGbaToGpio() const
{
    return direction;
}
