#include "sram.h"

Sram::Sram()
    : Save(Type::Sram)
{
    data.resize(kSize, 0xFF);
}
 
u8 Sram::read(u32 addr)
{
    return data[addr];
}

void Sram::write(u32 addr, u8 byte)
{
    Save::write(addr, byte);

    data[addr] = byte;
}

bool Sram::hasValidSize() const
{
    return data.size() == kSize;
}
