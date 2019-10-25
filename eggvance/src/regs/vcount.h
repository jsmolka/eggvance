#pragma once

#include "register.h"

class VCount
{
public:
    operator int() const;
    VCount& operator=(int value);

    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int line;
};
