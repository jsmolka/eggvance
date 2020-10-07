#include "header.h"

#include <numeric>

#include <shell/algorithm.h>

template<uint N>
static std::string makeAscii(const u8 (&data)[N])
{
    std::string ascii(reinterpret_cast<const char*>(data), N);

    shell::trimIf(ascii, shell::IsCntrl<char>());

    return ascii;
}

Header::Header(const std::vector<u8>& rom)
{
    if (rom.size() < kSize)
        return;

    const Data& data = *reinterpret_cast<const Data*>(rom.data());

    if (data.fixed_96h == 0x96 && data.complement == complement(data))
    {
        title = makeAscii(data.game_title);
        code  = makeAscii(data.game_code);
    }
}

u8 Header::complement(const Data& data)
{
    const u8* beg = reinterpret_cast<const u8*>(&data.game_title);
    const u8* end = reinterpret_cast<const u8*>(&data.complement);

    return -(std::accumulate<const u8*, u8>(beg, end, 0) + 0x19);
}
