#include "sram.h"

inline constexpr auto kSize = 32 * 1024;

Sram::Sram()
    : Save(Type::Sram)
{
    data.resize(kSize, 0xFF);
}
 
void Sram::reset()
{

}

u8 Sram::read(u32 addr)
{
    return data[addr];
}

void Sram::write(u32 addr, u8 byte)
{
    changed = true;
    data[addr] = byte;
}

bool Sram::isValidSize(uint size) const
{
    return size == kSize;
}
