#include "bios.h"

#include <shell/hash.h>

#include "bios_normmatt.h"
#include "arm/arm.h"
#include "base/config.h"
#include "base/utility.h"

void Bios::init(const fs::path& path)
{
    if (path.empty())
    {
        static_assert(kNormmattBios.size() == data.size());

        std::copy(kNormmattBios.begin(), kNormmattBios.end(), data.begin());
    }
    else
    {
        if (!fs::read(path, data))
            exit("Cannot read BIOS: {}", path);

        if (config.bios_hash)
        {
            constexpr std::size_t kExpected = 0x860D'7AFF'82E9'94DC;

            if (shell::hashRange(data.begin(), data.end()) != kExpected)
                exit("Invalid BIOS hash");
        }
    }
}

u8 Bios::readByte(u32 addr)
{
    if (arm.pc < kSize)
        return data.readFast<u8>(addr);
    else
        return previous >> (addr & 0x3);
}

u16 Bios::readHalf(u32 addr)
{
    if (arm.pc < kSize)
        return data.readFast<u16>(addr);
    else
        return previous >> (addr & 0x2);
}

u32 Bios::readWord(u32 addr)
{
    if (arm.pc < kSize)
        return previous = data.readFast<u32>(addr);
    else
        return previous;
}
