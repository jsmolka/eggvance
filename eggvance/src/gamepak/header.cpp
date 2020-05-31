#include "header.h"

#include <algorithm>
#include <numeric>

template<uint N>
static std::string makePrintable(u8(&data)[N])
{
    std::string str(reinterpret_cast<char*>(data), N);
    str.erase(std::find(str.begin(), str.end(), '\0'), str.end());

    return str;
}

Header::Header(const std::vector<u8>& rom)
{
    if (rom.size() < sizeof(HeaderData))
        return;

    std::copy_n(rom.begin(), sizeof(HeaderData), reinterpret_cast<u8*>(&header));

    title = makePrintable(header.game_title);
    code  = makePrintable(header.game_code);

    u8 expected = complement();
}

u8 Header::complement() const
{
    const u8* beg = reinterpret_cast<const u8*>(&header) + 0xA0;
    const u8* end = reinterpret_cast<const u8*>(&header) + 0xBD;

    return -(std::accumulate<const u8*, u8>(beg, end, 0) + 0x19);
}
