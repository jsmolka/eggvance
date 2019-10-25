#pragma once

#include "register.h"

class IRQRequest : public Register<2>
{
public:
    operator int() const;
    IRQRequest& operator|=(int value);

    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int value;
};
