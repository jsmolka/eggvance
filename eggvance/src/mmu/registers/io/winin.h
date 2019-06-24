#pragma once

#include "../register.h"

class Winin : public Register<u16>
{
public:
    Winin(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 1> win0_bg0;  // Window 0 BG0 enabled
    BitField<u16,  1, 1> win0_bg1;  // Window 0 BG1 enabled
    BitField<u16,  2, 1> win0_bg2;  // Window 0 BG2 enabled
    BitField<u16,  3, 1> win0_bg3;  // Window 0 BG3 enabled
    BitField<u16,  4, 1> win0_obj;  // Window 0 OBJ enabled
    BitField<u16,  5, 1> win0_sfx;  // Window 0 SFX enabled
    BitField<u16,  8, 1> win1_bg0;  // Window 1 BG0 enabled
    BitField<u16,  9, 1> win1_bg1;  // Window 1 BG1 enabled
    BitField<u16, 10, 1> win1_bg2;  // Window 1 BG2 enabled
    BitField<u16, 11, 1> win1_bg3;  // Window 1 BG3 enabled
    BitField<u16, 12, 1> win1_obj;  // Window 1 OBJ enabled
    BitField<u16, 13, 1> win1_sfx;  // Window 1 SFX enabled
};
