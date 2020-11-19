#include "rom.h"

#include <numeric>

#include <shell/algorithm.h>
#include <shell/utility.h>

#include "base/bit.h"
#include "base/panic.h"

template<uint N>
std::string makeAscii(const u8 (&data)[N])
{
    std::string ascii(reinterpret_cast<const char*>(data), N);

    shell::trimIf(ascii, shell::IsCntrl<char>());

    return ascii;
}

Rom::Rom(const fs::path& file)
{
    if (!fs::read(file, data))
        panic("Cannot read ROM {}", file);

    size = data.size();

    if (size < kHeaderSize || size > kMaxSize)
        panic("Bad ROM size");

    data.reserve(kMaxSize);

    for (auto x = data.size(); x < data.capacity(); ++x)
    {
        data.push_back(bit::byte(x >> 1, x & 0x1));
    }

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
