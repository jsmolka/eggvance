#pragma once

#include "register.h"

class BGParameter : public Register<2>
{
public:
    void reset();

    void write(int index, u8 byte);

    s16 value;
};
