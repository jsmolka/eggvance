#pragma once

#include "register.h"

class KeyInput : public Register<2>
{
public:
    void reset();

    operator int() const;
    KeyInput& operator|=(int value);
    KeyInput& operator&=(int value);

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int keys;
};
