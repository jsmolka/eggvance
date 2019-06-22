#pragma once

#include "../register.h"

class Mosaic : public Register<u16>
{
public:
    Mosaic(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16,  0, 4> bg_x;      // BG x stretch (minus 1)
    BitField<u16,  4, 4> bg_y;      // BG y stretch (minus 1)
    BitField<u16,  8, 4> sprite_x;  // Sprite x stretch (minus 1)
    BitField<u16, 12, 4> sprite_y;  // Sprite y stretch (minus 1)
};
