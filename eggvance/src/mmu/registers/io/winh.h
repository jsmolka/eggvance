#pragma once

#include "../register.h"

class Winh : public Register<u16>
{
public:
    Winh(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16, 0, 8> x2;  // Right coordiante of window, plus 1 (x2 > 240 or x1 > x2 -> x2 = 240)
    BitField<u16, 8, 8> x1;  // Left coordinate of window
};
