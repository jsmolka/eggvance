#pragma once

#include "register.h"

class VCount : public Register<2>
{
public:
    operator int() const;

    void reset();

    u8 read(int index);

    void next();
};
