#pragma once

#include "register.h"

class BGReference : public Register<4>
{
public:
    void reset();

    void writeByte(int index, u8 byte);

    int reference;
    int internal;
};
