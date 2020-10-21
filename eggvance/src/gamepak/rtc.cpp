#include "rtc.h"

Rtc::Rtc()
    : Gpio(Type::Rtc)
{

}

void Rtc::reset()
{

}

u16 Rtc::read(u32 addr)
{
    return 0;
}

void Rtc::write(u32 addr, u16 half)
{

}
