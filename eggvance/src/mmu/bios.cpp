#include "bios.h"

#include <cstring>
#include <fstream>
#include <stdexcept>

#include "arm/arm.h"
#include "base/config.h"

void BIOS::reset()
{
    if (config.bios_skip)
        last_fetched = data.readFast<u32>(0xE4);
    else
        last_fetched = 0;
}

void BIOS::init(const Path& file)
{
    if (file.empty())
    {
        std::memcpy(data.data<u8>(0), replacement_bios.data(), 0x4000);
    }
    else
    {
        static constexpr u64 expected_hash = 0xECCF5E4CEA50816E;

        if (!read(config.bios_file))
            throw std::runtime_error("Cannot read BIOS");

        if (hash(data.data<u32>(0), 0x1000) != expected_hash)
            throw std::runtime_error("Invalid BIOS hash");
    }
}

u8 BIOS::readByte(u32 addr)
{
    if (arm.pc < data.size())
        return data.readByte(addr);
    else
        return readProtected(addr);
}

u16 BIOS::readHalf(u32 addr)
{
    if (arm.pc < data.size())
        return data.readHalf(addr);
    else
        return readProtected(addr);
}

u32 BIOS::readWord(u32 addr)
{
    if (arm.pc < data.size())
        return last_fetched = data.readWord(addr);
    else
        return readProtected(addr);
}

u32 BIOS::readProtected(u32 addr) const
{
    return last_fetched >> (addr & 0x3);
}

bool BIOS::read(const Path& file)
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
