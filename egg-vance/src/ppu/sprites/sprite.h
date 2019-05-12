#pragma once

#include <array>

#include "common/integer.h"

struct Sprite
{
    bool disabled;

    int x;
    int y;
    int palette;

    int width;
    int height;

    std::array<std::array<u8, 128>, 128> data;
};

