#pragma once

#include "common/integer.h"

struct BlendAlpha
{
    void write(int index, u8 byte);

    int eva;  // EVA coefficient for layer A (0..16)
    int evb;  // EVA coefficient for layer B (0..16)
};
