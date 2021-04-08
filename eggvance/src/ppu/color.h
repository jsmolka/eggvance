#pragma once

#include <shell/array.h>

#include "base/int.h"

class Color
{
public:
    Color() = delete;

    static void init(bool color_correct);
    static uint toArgb(u16 color);

private:
    inline static shell::array<u32, 0x8000> lut = {};
};
