#pragma once

#include "../register.h"

class Bldcnt : public Register<u16>
{
public:
    Bldcnt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16,  0, 1> a_bg0;  // Top layer BG0
    BitField<u16,  1, 1> a_bg1;  // Top layer BG1
    BitField<u16,  2, 1> a_bg2;  // Top layer BG2
    BitField<u16,  3, 1> a_bg3;  // Top layer BG3
    BitField<u16,  4, 1> a_obj;  // Top layer sprites
    BitField<u16,  5, 1> a_bd;   // Top layer backdrop
    BitField<u16,  6, 2> mode;   // Mode (0 = none, 1 = alpha blending, 2 = brightness increase, 3 = brightness decrease)
    BitField<u16,  8, 1> b_bg0;  // Bottom layer BG0
    BitField<u16,  9, 1> b_bg1;  // Bottom layer BG1
    BitField<u16, 10, 1> b_bg2;  // Bottom layer BG2
    BitField<u16, 11, 1> b_bg3;  // Bottom layer BG3
    BitField<u16, 12, 1> b_obj;  // Bottom layer sprites
    BitField<u16, 13, 1> b_bd;   // Bottom layer backdrop
};
