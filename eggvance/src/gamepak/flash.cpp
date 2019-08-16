#include "flash.h"

Flash::Flash(const std::string& file, Save::Type type)
    : Save(file, type)
{

}

Flash::~Flash()
{

}

u8 Flash::readByte(u32 addr)
{
    return 0;
}

void Flash::writeByte(u32 addr, u8 byte)
{

}
