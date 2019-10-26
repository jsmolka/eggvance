#pragma once

#include "register.h"

class IRQRequest : public Register<2>
{
public:
    operator int() const;
    IRQRequest& operator|=(int value);

    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int request;
};
