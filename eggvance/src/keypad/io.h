#pragma once

#include "base/register.h"

class KeyInput : public XRegisterR<u16, 0x03FF>
{
public:
    KeyInput();

    KeyInput& operator=(u16 value);
    operator u16() const;
};

class KeyControl : public XRegister<u16, 0xC3FF>
{
public:
    void write(uint index, u8 byte);

    uint mask = 0;
    uint irq  = 0;
    uint cond = 0;
};
