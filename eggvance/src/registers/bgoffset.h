#pragma once

#include "register.h"

class BGOffset : public Register<2>
{
public:
    void reset();

    void write(int index, u8 byte);

    int value;
};
