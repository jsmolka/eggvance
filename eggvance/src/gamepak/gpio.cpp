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

u8 Gpio::read(u32 addr)
{
    switch (addr)
    {
    case kAddressData:
    {
        uint value = readPort() & maskGpioToGba();

        data &= maskGbaToGpio();
        data |= value;

        return value;
    }

    case kAddressDirection:
        return direction;

    case kAddressReadable:
        return readable;
    }
    return 0;
}

void Gpio::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    case kAddressData:
        data &= maskGpioToGba();
        data |= maskGbaToGpio() & byte;

        writePort(data);
        break;

    case kAddressDirection:
        direction = byte & 0xF;
        break;

    case kAddressReadable:
        readable = byte & 0x1;
        break;
    }
}

u8 Gpio::readPort()
{
    return 0;
}

void Gpio::writePort(u8 data)
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
    return ~maskGbaToGpio();
}

uint Gpio::maskGbaToGpio() const
{
    return direction & 0xF;
}
