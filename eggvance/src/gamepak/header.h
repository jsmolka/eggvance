#pragma once

#include <string>
#include <vector>

#include "base/int.h"

class Header
{
private:
    struct Data
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
    static constexpr uint kSize = sizeof(Data);

    Header() = default;
    explicit Header(const std::vector<u8>& rom);

    std::string title;
    std::string code;

private:
    static u8 complement(const Data& data);
};
