#include "bios.h"

#include <fstream>

BIOS::BIOS(const std::string& file) 
{
    static constexpr u64 expected_hash = 0xECCF5E4CEA50816E;

    valid = read(file);
    valid = valid && hash(data.ptr<u32>(0), 0x1000) == expected_hash;

    reset();
}

void BIOS::reset()
{
    last_fetched = LF_BRANCH_ROM;
}

void BIOS::setPC(u32* pc)
{
    this->pc = pc;
}

u8 BIOS::readByte(u32 addr)
{
    protect(addr);
    return data.readByte(addr);
}

u16 BIOS::readHalf(u32 addr)
{
    protect(addr);
    return data.readHalf(addr);
}

u32 BIOS::readWord(u32 addr)
{
    switch (addr)
    {
    case LF_BRANCH_ROM:
    case LF_BRANCH_IRQ:
    case LF_RETURN_IRQ:
    case LF_RETURN_SWI:
        last_fetched = addr + 8;
        break;

    default:
        protect(addr);
        break;
    }
    return data.readWord(addr);
}

void BIOS::protect(u32& addr)
{
    if (*pc >= data.size())
        addr = last_fetched;
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

    stream.read(data.ptr<char>(0), size);

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
