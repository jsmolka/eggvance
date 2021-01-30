#pragma once

#include "base/int.h"

class Length
{
public:
    Length(uint base);

    Length& operator=(uint value);

    void init();
    void tick();
    uint enabled() const;

    uint base   = 0;
    uint timer  = 0;
    uint length = 0;
    uint expire = 0;
};
