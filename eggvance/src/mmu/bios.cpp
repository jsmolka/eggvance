#include "bios.h"

#include <shell/hash.h>

#include "arm/arm.h"
#include "base/config.h"
#include "base/log.h"
#include "base/panic.h"
#include "bios_normmatt.h"

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
            panic("Cannot read bios {}", path);

        if (config.bios_hash)
        {
            constexpr std::size_t kHash = 0x860D'7AFF'82E9'94DC;

            if (shell::hashRange(data.begin(), data.end()) != kHash)
                panic("Bad bios hash");
        }
    }
}

u8  Bios::readByte(u32 addr) { return read<u8 >(addr); }
u16 Bios::readHalf(u32 addr) { return read<u16>(addr); }
u32 Bios::readWord(u32 addr) { return read<u32>(addr); }

template<typename Integral>
Integral Bios::read(u32 addr)
{
    constexpr auto kMask = ~(sizeof(Integral) - 1);

    if (arm.pc < kSize)
    {
        auto value = data.readFast<Integral>(addr & kMask);

        if (std::is_same_v<Integral, u32>)
            previous = value;

        return value;
    }

    SHELL_LOG_WARN("Bad pc {:08X}", arm.pc);

    return previous >> (8 * (addr & 0x3));
}
