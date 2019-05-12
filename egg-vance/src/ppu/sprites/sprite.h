#pragma once

#include <array>

#include "common/integer.h"

class Sprite
{
public:
    void setPixel(int x, int y, int value, bool flip_x, bool flip_y);

    bool disabled;

    int x;
    int y;
    int palette;

    int width;
    int height;

    std::array<std::array<u8, 128>, 128> data;
};

