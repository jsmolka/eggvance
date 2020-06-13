#include "sram.h"

Sram::Sram(const fs::path& file)
    : Save(file, Type::Sram)
{
    data.resize(kSize, 0xFF);
}
 
u8 Sram::read(u32 addr)
{
    return data[addr];
}

void Sram::write(u32 addr, u8 byte)
{
    data[addr] = byte;
}
