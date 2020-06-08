#include "rtc.h"

Rtc::Rtc()
    : Gpio(Type::Rtc)
{

}

u16 Rtc::read(u32 addr)
{
    return 0;
}

void Rtc::write(u32 addr, u16 half)
{

}
