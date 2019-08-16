#include "dummy.h"

Dummy::Dummy()
    : Save("", Save::Type::NONE)
{

}

u8 Dummy::readByte(u32 addr)
{
    return 0;
}

void Dummy::writeByte(u32 addr, u8 byte)
{

}
