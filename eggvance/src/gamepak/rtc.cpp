#include "rtc.h"

#include <shell/fmt/format.h>

Rtc::Rtc()
    : Gpio(Type::Rtc)
{
    reset();
}

void Rtc::reset()
{
    Gpio::reset();
}

u16 Rtc::readPort()
{
    return u16();
}

void Rtc::writePort(u16 half)
{

}
