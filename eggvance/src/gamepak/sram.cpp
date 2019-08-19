#include "sram.h"

SRAM::SRAM(const std::string& file)
    : Save(file, Save::Type::SRAM)
{
    data.resize(0x8000, 0);
}

u8 SRAM::readByte(u32 addr)
{
    return data[addr];
}

void SRAM::writeByte(u32 addr, u8 byte)
{
    data[addr] = byte;
}
