#pragma once

#include "register.h"

class IRQMaster : public Register<4>
{
public:
    operator bool() const;

    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);
};
