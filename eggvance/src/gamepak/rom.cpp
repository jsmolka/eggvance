#include "rom.h"

#include <numeric>

#include <shell/algorithm.h>

#include "base/panic.h"

template<uint N>
static std::string makeAscii(const u8 (&data)[N])
{
    std::string ascii(reinterpret_cast<const char*>(data), N);

    shell::trimIf(ascii, shell::IsCntrl<char>());

    return ascii;
}

template<typename Integral>
Integral intceil(Integral value, Integral multiple)
{
    return multiple * (value + multiple - 1) / multiple;
}

void Rom::load(const fs::path& file)
{
    if (!fs::read(file, data))
        panic("Cannot read ROM {}", file);

    if (data.size() < kHeaderSize || data.size() > kSize)
        panic("Bad ROM size");

    data.resize(intceil(data.size(), sizeof(u32)), 0);

    const Header& header = *reinterpret_cast<const Header*>(data.data());

    if (header.fixed_96h == 0x96 && header.complement == complement(header))
    {
        title = makeAscii(header.game_title);
        code  = makeAscii(header.game_code);
    }
}

u8 Rom::complement(const Header& header)
{
    const u8* beg = reinterpret_cast<const u8*>(&header.game_title);
    const u8* end = reinterpret_cast<const u8*>(&header.complement);

    return -(std::accumulate<const u8*, u8>(beg, end, 0) + 0x19);
}
