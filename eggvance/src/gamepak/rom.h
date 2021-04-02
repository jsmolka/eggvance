#pragma once

#include <string>
#include <vector>

#include "base/int.h"
#include "base/filesystem.h"

class Rom
{
private:
    struct Header
    {
        u8 entry_point[4];
        u8 nintendo_logo[156];
        u8 game_title[12];
        u8 game_code[4];
        u8 maker_code[2];
        u8 fixed_96h;
        u8 unit_code;
        u8 device_type;
        u8 reserved[7];
        u8 game_version;
        u8 complement;
        u8 checksum[2];
    };

public:
    static constexpr auto kSizeMax = 32 * 1024 * 1024;
    static constexpr auto kSizeHeader = sizeof(Header);

    Rom();

    void init(const fs::path& file);

    template<typename Integral>
    Integral read(u32 addr) const
    {
        static_assert(std::is_integral_v<Integral>);

        return *reinterpret_cast<const Integral*>(data.data() + addr);
    }

    uint size = 0;
    uint mask = kSizeMax;
    std::string code;
    std::string title;
    std::vector<u8> data;

private:
    static u8 complement(const Header& header);
};
