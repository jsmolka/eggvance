#pragma once

#include "register.h"

class KeyInput : public Register<2>
{
public:
    KeyInput& operator=(u16 value);
    operator u16() const;

    void reset();

    u8 read(int index);

private:
    u16 value;
};
