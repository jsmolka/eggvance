#include "none.h"

None::None()
    : Save("", Save::Type::NONE)
{

}

u8 None::readByte(u32 addr)
{
    return 0;
}

void None::writeByte(u32 addr, u8 byte)
{

}
