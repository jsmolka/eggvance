#include "dmaaddress.h"

DmaAddress& DmaAddress::operator=(u32 value)
{
    data = value;
    return *this;
}

DmaAddress::operator u32() const
{
    return data;
}

bool DmaAddress::isFifoA() const
{
    return data == 0x400'00A0;
}

bool DmaAddress::isFifoB() const
{
    return data == 0x400'00A4;
}

bool DmaAddress::isGamePak() const
{
    return data >= 0x800'0000 && data < 0xE00'0000;
}
