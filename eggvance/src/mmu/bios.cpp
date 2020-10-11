#include "bios.h"

#include <shell/hash.h>

#include "arm/arm.h"
#include "base/config.h"
#include "base/utility.h"
#include "mmu/bios_normmatt.h"

void BIOS::reset()
{
    previous = 0xE129'F000;
}

void BIOS::init(const fs::path& path)
{
    if (path.empty())
    {
        std::copy(kNormmattBios.begin(), kNormmattBios.end(), data.begin());
    }
    else
    {
        if (!fs::read(path, data))
            exit("Cannot read BIOS: {}", path);

        if (config.bios_hash)
        {
            constexpr u64 kExpected = 0x860D'7AFF'82E9'94DC;

            if (shell::hashRange(data.begin(), data.end()) != kExpected)
                exit("Invalid BIOS hash");
        }
    }
}

u8 BIOS::readByte(u32 addr)
{
    if (arm.pc < data.size())
        return data.readByte(addr);
    else
        return previous >> (addr & 0x3);
}

u16 BIOS::readHalf(u32 addr)
{
    if (arm.pc < data.size())
        return data.readHalf(addr);
    else
        return previous >> (addr & 0x2);
}

u32 BIOS::readWord(u32 addr)
{
    if (arm.pc < data.size())
        return previous = data.readWord(addr);
    else
        return previous;
}
