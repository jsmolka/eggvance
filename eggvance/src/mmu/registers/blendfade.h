#pragma once

#include "common/integer.h"

struct BlendFade
{
    void write(u8 byte);

    int evy;  // EVY coefficient for brightness (0..16)
};
