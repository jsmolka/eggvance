#include "bios.h"

#include <fstream>

#include "arm/arm.h"
#include "common/config.h"
#include "common/message.h"

void BIOS::reset()
{
    if (config.bios_skip)
        last_fetched = data.readWordFast(0xE4);
    else
        last_fetched = 0;
}

bool BIOS::init(const std::string& file)
{
    static constexpr u64 expected_hash = 0xECCF5E4CEA50816E;

    if (!read(file))
    {
        showMessage("Cannot read BIOS file.");
        return false;
    }
    if (hash(data.data<u32>(0), 0x1000) != expected_hash)
    {
        showMessage("BIOS hash is invalid.");
        return false;
    }
    return true;
}

u8 BIOS::readByte(u32 addr)
{
    if (arm.pc < data.size())
        return data.readByte(addr);
    else
        return last_fetched;
}

u16 BIOS::readHalf(u32 addr)
{
    if (arm.pc < data.size())
        return data.readHalf(addr);
    else
        return last_fetched;
}

u32 BIOS::readWord(u32 addr)
{
    if (arm.pc < data.size())
        return last_fetched = data.readWord(addr);
    else
        return last_fetched;
}

bool BIOS::read(const std::string& file)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    if (size != data.size())
        return false;

    stream.read(data.data<char>(0), size);

    return true;
}

u64 BIOS::hash(u32* data, int size)
{
    u64 seed = 0;
    while (size--)
    {
        seed ^= *data + 0x9E3779B9 + (seed << 6) + (seed >> 2);
        data++;
    }
    return seed;
}
