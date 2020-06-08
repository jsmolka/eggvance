#include "gpio.h"

Gpio::Gpio()
    : type(Type::None)
{

}

Gpio::Gpio(Type type)
    : type(type)
{

}

u16 Gpio::read(u32 addr)
{
    return 0;
}

void Gpio::write(u32 addr, u16 half)
{

}
