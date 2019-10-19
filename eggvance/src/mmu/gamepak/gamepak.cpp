#include "gamepak.h"

#include "fileutil.h"

u8 GamePak::readByte(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u8*>(&data[addr]);
    else
        return 0;
}

u16 GamePak::readHalf(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u16*>(&data[addr]);
    else
        return 0;
}

u32 GamePak::readWord(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u32*>(&data[addr]);
    else
        return 0;
}

// Todo: check if cartridge exists
bool GamePak::load(const std::string& file)
{
    if (this->file != file)
    {
        if (!fileutil::read(file, data))
            return false;
    }
    return true;
}

std::size_t GamePak::size() const
{
    return data.size();
}
