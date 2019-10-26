#pragma once

#include "register.h"

class IRQMaster : public Register<4>
{
public:
    operator bool() const;

    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int master;
};
