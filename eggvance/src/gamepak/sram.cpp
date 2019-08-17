#include "sram.h"

#include "file.h"

SRAM::SRAM(const std::string& file)
    : Save(file, Save::Type::SRAM)
{
    data.resize(0x8000, 0);

    if (File::exists(file))
        File::read(file, data);
}

SRAM::~SRAM()
{
    File::write(file, data);
}

u8 SRAM::readByte(u32 addr)
{
    return data[addr];
}

void SRAM::writeByte(u32 addr, u8 byte)
{
    data[addr] = byte;
}
