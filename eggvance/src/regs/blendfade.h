#pragma once

#include "register.h"

class BlendFade : public Register<2>
{
public:
    void reset();

    void writeByte(int index, u8 byte);

    int evy;
};
