#include "cartridge.h"

#include <fstream>

u8 Cartridge::readByte(u32 addr)
{
    if (data.size() < addr)
        return *reinterpret_cast<u8*>(&data[addr]);
    else
        return 0;
}

u16 Cartridge::readHalf(u32 addr)
{
    if (data.size() < addr)
        return *reinterpret_cast<u16*>(&data[addr]);
    else
        return 0;
}

u32 Cartridge::readWord(u32 addr)
{
    if (data.size() < addr)
        return *reinterpret_cast<u32*>(&data[addr]);
    else
        return 0;
}

bool Cartridge::load(const std::string& path)
{
    if (this->path != path)
    {

    }
    return true;
}

auto Cartridge::size() const -> std::size_t
{
    return data.size();
}

bool Cartridge::FileRead(const std::string& path, std::vector<u8>& dst)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    dst.resize(size);
    stream.read(reinterpret_cast<char*>(dst.data()), size);

    return true;
}

bool Cartridge::FileWrite(const std::string& path, std::vector<u8>& src)
{
    std::ofstream stream(path, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.write(reinterpret_cast<char*>(src.data()), src.size());

    return true;
}
