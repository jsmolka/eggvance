#include "rom.h"

#include <numeric>
#include <shell/algorithm.h>
#include <shell/errors.h>
#include <shell/utility.h>

#include "base/bit.h"

template<uint N>
std::string makeAscii(const u8 (&data)[N])
{
    std::string ascii(reinterpret_cast<const char*>(data), N);

    shell::trimIf(ascii, shell::IsCntrl<char>());

    return ascii;
}

Rom::Rom()
{
    data.reserve(kSizeMax);
}

void Rom::init(const fs::path& file)
{
    if (fs::read(file, data) != fs::Status::Ok)
        throw shell::Error("Cannot read ROM: {}", file);

    size = data.size();
    mask = kSizeMax;

    if (size < kSizeHeader || size > kSizeMax)
        throw shell::Error("Bad ROM size: {}", size);

    for (auto x = size; x < data.capacity(); ++x)
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
