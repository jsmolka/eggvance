#pragma once

#include <string>
#include <vector>

#include "base/filesystem.h"
#include "base/int.h"

class Rom : public std::vector<u8>
{
public:
    static constexpr auto kMaxSize = 32 * 1024 * 1024;

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

    Rom();

    bool load(const fs::path& file);

    template<typename Integral>
    Integral read(u32 addr) const
    {
        static_assert(std::is_integral_v<Integral>);

        return *reinterpret_cast<const Integral*>(this->data() + addr);
    }

    fs::path file;
    std::size_t mask = 0;
    std::string code;
    std::string title;

private:
    static u8 complement(const Header& header);
};
