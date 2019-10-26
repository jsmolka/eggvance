#pragma once

#include "register.h"

class DMACount : public Register<2>
{
public:
    void reset();

    void write(int index, u8 byte);

    int count();

    int limit;
};
