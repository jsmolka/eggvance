#pragma once

#include "../register.h"

class Winv : public Register<u16>
{
public:
    Winv(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16, 0, 8> y2;  // Bottom coordiante of window, plus 1 (y2 > 160 or y1 > y2 -> y2 = 240)
    BitField<u16, 8, 8> y1;  // Top coordinate of window
};
