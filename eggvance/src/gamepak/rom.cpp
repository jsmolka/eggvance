#include "rom.h"

#include <numeric>
#include <shell/algorithm.h>
#include <shell/errors.h>

#include "base/bit.h"

template<std::size_t N>
std::string makeAscii(const u8 (&data)[N])
{
    std::string ascii(reinterpret_cast<const char*>(data), N);

    shell::trimIf(ascii, shell::IsCntrl<char>());

    return ascii;
}

Rom::Rom()
{
    reserve(kMaxSize);
}

void Rom::init(const fs::path& file)
{
    if (fs::read(file, *this) != fs::Status::Ok)
        throw shell::Error("Cannot read ROM: {}", file);

    mask = kMaxSize;

    if (size() <= sizeof(Header) || size() > kMaxSize)
        throw shell::Error("Bad ROM size: {}", size());

    for (std::size_t addr = size(); addr < capacity(); ++addr)
    {
        data()[addr] = bit::byte(addr >> 1, addr & 0x1);
    }

    const Header& header = *reinterpret_cast<const Header*>(data());

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
