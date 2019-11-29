#pragma once

#include "register.h"

class IntEnabled : public Register<2>
{
public:
    operator int() const;

    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int enabled;
};
