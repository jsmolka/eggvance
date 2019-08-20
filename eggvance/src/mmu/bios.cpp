#include "bios.h"

#include <fstream>

BIOS::BIOS(const std::string& file)
{
    static constexpr u64 expected_hash = 0xECCF5E4CEA50816E;

    valid = readFile(file);
    valid = valid && hash(data.ptr<u32>(0), 0x1000) == expected_hash;
}

u8 BIOS::readByte(u32 addr)
{
    return read<u8>(addr);
}

u16 BIOS::readHalf(u32 addr)
{
    return read<u16>(addr);
}

u32 BIOS::readWord(u32 addr)
{
    return read<u32>(addr);
}

template<typename T>
inline T BIOS::read(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<T*>(&data[addr]);
    else
        return 0;
}

bool BIOS::readFile(const std::string& file)
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
