#include "gpio.h"

#include "base/bit.h"

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
    readable  = 0;
    direction = 0;
    data      = 0;
}

u16 Gpio::readHalf(u32 addr)
{
    addr &= ~0x1;

    switch (addr)
    {
    case kPortData:
    {
        uint value = readPort() & maskGpioToGba();

        data &= maskGbaToGpio();
        data |= value;

        return value;
    }

    case kPortDirection:
        return direction;

    case kPortReadEnable:
        return readable;
    }
    return 0;
}

u32 Gpio::readWord(u32 addr)
{
    addr &= ~0x3;

    u32 value = 0;
    value |= readHalf(addr + 0) <<  0;
    value |= readHalf(addr + 2) << 16;

    return value;
}

void Gpio::writeHalf(u32 addr, u16 half)
{
    addr &= ~0x1;

    switch (addr)
    {
    case kPortData:
        data &= maskGpioToGba();
        data |= maskGbaToGpio() & half;

        writePort(data);
        break;

    case kPortDirection:
        direction = half & 0xF;
        break;

    case kPortReadEnable:
        readable = half & 0x1;
        break;
    }
}

void Gpio::writeWord(u32 addr, u32 word)
{
    addr &= ~0x3;

    writeHalf(addr + 0, bit::seq< 0, 16>(word));
    writeHalf(addr + 2, bit::seq<16, 16>(word));
}

u16 Gpio::readPort()
{
    return 0;
}

void Gpio::writePort(u16 half)
{

}

uint Gpio::maskGpioToGba() const
{
    return ~direction;
}

uint Gpio::maskGbaToGpio() const
{
    return direction;
}
