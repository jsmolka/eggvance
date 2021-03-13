#pragma once

#include "base/register.h"

class SioControl : public XRegister<u16>
{
public:
    void write(uint index, u8 byte);

    uint enabled = 0;
    uint irq     = 0;
};
