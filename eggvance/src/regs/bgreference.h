#pragma once

#include "register.h"

class BGReference : public Register<4>
{
public:
    void reset();

    void write(int index, u8 byte);

    int value;
    int current;
};
