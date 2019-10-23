#include "bios.h"

#include <fstream>
#include <SDL2/SDL_messagebox.h>

#include "arm/arm.h"
#include "common/config.h"

void BIOS::reset()
{
    last = data.readWord(0xDC);
}

bool BIOS::init()
{
    static constexpr u64 expected_hash = 0xECCF5E4CEA50816E;

    if (!read(config.bios_file))
    {
        SDL_ShowSimpleMessageBox(0, "Missing BIOS", "Please place a GBA bios.bin next to the emulator.", nullptr);
        return false;
    }
    if (hash(data.data<u32>(0), 0x1000) != expected_hash)
    {
        SDL_ShowSimpleMessageBox(0, "Invalid BIOS", "The BIOS does not match the requirements.", nullptr);
        return false;
    }

    return true;
}

u8 BIOS::readByte(u32 addr)
{
    if (arm.pc >= data.size())
        return static_cast<u8>(last);
    else
        return data.readByte(addr);
}

u16 BIOS::readHalf(u32 addr)
{
    if (arm.pc >= data.size())
        return static_cast<u16>(last);
    else
        return data.readHalf(addr);
}

u32 BIOS::readWord(u32 addr)
{
    if (arm.pc >= data.size())
        return last;
    else
        return last = data.readWord(addr);
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
