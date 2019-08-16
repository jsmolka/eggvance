#include "eeprom.h"

EEPROM::EEPROM(const std::string& file)
    : Save(file, Save::Type::EEPROM)
{

}

EEPROM::~EEPROM()
{

}

u8 EEPROM::readByte(u32 addr)
{
    return 0;
}

void EEPROM::writeByte(u32 addr, u8 byte)
{

}
