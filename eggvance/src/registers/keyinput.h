#pragma once

#include "register.h"

class KeyInput : public Register<2>
{
public:
    operator int() const;
    KeyInput& operator|=(int value);
    KeyInput& operator&=(int value);

    void reset();

    u8 read(int index);

    u16 value;
};
