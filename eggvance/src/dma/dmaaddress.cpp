#include "dmaaddress.h"

DmaAddress& DmaAddress::operator=(uint value)
{
    data = value;

    return *this;
}

DmaAddress::operator uint() const
{
    return data;
}

bool DmaAddress::fifoA() const
{
    return data == 0x400'00A0;
}

bool DmaAddress::fifoB() const
{
    return data == 0x400'00A4;
}

bool DmaAddress::gamepak() const
{
    return data >= 0x800'0000 && data < 0xE00'0000;
}
