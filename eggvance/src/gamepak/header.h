#pragma once

#include <string>
#include <vector>

#include "base/integer.h"

class Header
{
public:
    Header() = default;
    explicit Header(const std::vector<u8>& rom);

    std::string title;
    std::string code;

private:
    struct Data
    {
        u32 entry_point;
        u8  nintendo_logo[156];
        u8  game_title[12];
        u8  game_code[4];
        u8  maker_code[2];
        u8  fixed_96h;
        u8  unit_code;
        u8  device_type;
        u8  reserved_area[7];
        u8  game_version;
        u8  complement;
        u16 checksum;
    };

    static u8 complement(const Data& data);
};
