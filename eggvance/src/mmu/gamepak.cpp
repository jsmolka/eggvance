#include "gamepak.h"

#include <fstream>

GamePak::GamePak(const std::string& file)
{
    valid = read(file);
    if (valid)
        parseHeader();
}

u8 GamePak::readByte(u32 addr) const
{
    return addr < data.size() ? data[addr] : 0;
}

u8 GamePak::readSaveByte(u32 addr) const
{
    return sram[addr];
}

void GamePak::writeSaveByte(u32 addr, u8 byte)
{
    sram[addr] = byte;
}

std::string GamePak::makeString(u8* data, int size)
{
    return std::string(reinterpret_cast<char*>(data), size);
}

bool GamePak::read(const std::string& file)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    data.resize(size);
    stream.read(reinterpret_cast<char*>(data.data()), size);

    return true;
}

void GamePak::parseHeader()
{
    header.title = makeString(&data[0xA0], 12);
    header.code  = makeString(&data[0xAC], 4);
    header.maker = makeString(&data[0xB0], 2);
}
