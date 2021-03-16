#pragma once

#include "base/register.h"

class RemoteControl : public XRegister<u16>
{
public:
    RemoteControl();
};

class SioControl : public XRegister<u16>
{
public:
    void write(uint index, u8 byte);

private:
    uint irq = 0;
};
