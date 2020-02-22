#pragma once

#include "register.h"

class HaltControl : public Register<1>
{
public:
    void reset();

    void write(int index, u8 byte);
};
