#pragma once

#include "../register.h"

class Winout : public Register<u16>
{
public:
    Winout(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 1> winout_bg0;  // Outside window BG0 enabled
    BitField<u16,  1, 1> winout_bg1;  // Outside window BG1 enabled
    BitField<u16,  2, 1> winout_bg2;  // Outside window BG2 enabled
    BitField<u16,  3, 1> winout_bg3;  // Outside window BG3 enabled
    BitField<u16,  4, 1> winout_obj;  // Outside window OBJ enabled
    BitField<u16,  5, 1> winout_sfx;  // Outside window SFX enabled
    BitField<u16,  8, 1> winobj_bg0;  // OBJ window BG0 enabled
    BitField<u16,  9, 1> winobj_bg1;  // OBJ window BG1 enabled
    BitField<u16, 10, 1> winobj_bg2;  // OBJ window BG2 enabled
    BitField<u16, 11, 1> winobj_bg3;  // OBJ window BG3 enabled
    BitField<u16, 12, 1> winobj_obj;  // OBJ window OBJ enabled
    BitField<u16, 13, 1> winobj_sfx;  // OBJ window SFX enabled
};
