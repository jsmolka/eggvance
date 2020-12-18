#include "bios.h"

#include <shell/hash.h>

#include "bios_normmatt.h"
#include "arm/arm.h"
#include "base/config.h"
#include "base/panic.h"

void Bios::init(const fs::path& path)
{
    if (path.empty())
    {
        static_assert(kNormmattBios.size() == data.size());

        std::copy(kNormmattBios.begin(), kNormmattBios.end(), data.begin());
    }
    else
    {
        fs::Status status = fs::read(path, data);

        switch (status)
        {
        case fs::Status::BadSize:
            panic("Bad BIOS size");
            break;

        case fs::Status::BadFile:
        case fs::Status::BadStream:
            panic("Cannot read BIOS {}", path);
            break;
        }

        if (config.bios_hash)
        {
            constexpr std::size_t kExpected = 0x860D'7AFF'82E9'94DC;

            if (shell::hashRange(data.begin(), data.end()) != kExpected)
                panic("Bad BIOS hash");
        }
    }
}

u8  Bios::readByte(u32 addr) { return read<u8 >(addr); }
u16 Bios::readHalf(u32 addr) { return read<u16>(addr); }
u32 Bios::readWord(u32 addr) { return read<u32>(addr); }

template<typename Integral>
Integral Bios::read(u32 addr)
{
    addr &= ~(sizeof(Integral) - 1);

    if (arm.pc < kSize)
        cache = data.readFast<u32>(addr & ~0x3);

    return cache >> (8 * (addr & 0x3));
}
