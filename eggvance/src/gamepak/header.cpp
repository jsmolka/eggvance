#include "header.h"

#include <algorithm>
#include <numeric>

template<uint N>
static std::string makePrintable(const u8 (&data)[N])
{
    std::string str(reinterpret_cast<const char*>(data), N);
    str.erase(std::find(str.begin(), str.end(), '\0'), str.end());

    return str;
}

Header::Header(const std::vector<u8>& rom)
{
    if (rom.size() < sizeof(Data))
        return;

    const Data& data = *reinterpret_cast<const Data*>(rom.data());

    if (data.fixed_96h == 0x96 && data.complement == complement(data))
    {
        title = makePrintable(data.game_title);
        code  = makePrintable(data.game_code);
    }
}

u8 Header::complement(const Data& data)
{
    const u8* beg = reinterpret_cast<const u8*>(&data.game_title);
    const u8* end = reinterpret_cast<const u8*>(&data.complement);

    return -(std::accumulate<const u8*, u8>(beg, end, 0) + 0x19);
}
