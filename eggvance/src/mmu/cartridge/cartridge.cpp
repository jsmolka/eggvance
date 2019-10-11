#include "cartridge.h"

#include "fileutil.h"

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

bool Cartridge::load(const std::string& file)
{
    if (this->file != file)
    {
        if (!fileutil::read(file, data))
            return false;
    }
    return true;
}

std::size_t Cartridge::size() const
{
    return data.size();
}
