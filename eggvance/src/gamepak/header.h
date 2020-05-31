#pragma once

#include <string>
#include <vector>

#include "base/integer.h"

class Header
{
public:
    Header() = default;
    Header(const std::vector<u8>& rom);

    std::string title;
    std::string code;

private:
    u8 complement() const;

    struct HeaderData
    {
        u32 branch;
        u8  logo[156];
        u8  game_title[12];
        u8  game_code[4];
        u8  maker_code[2];
        u8  fixed;
        u8  unit_code;
        u8  device_type;
        u8  unused[7];
        u8  game_version;
        u8  complement;
        u16 checksum;
    } header = { 0 };
};
