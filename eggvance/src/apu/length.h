#pragma once

#include "base/int.h"

class Length
{
public:
    void init();
    void tick();

    uint value   = 0;
    uint initial = 0;
    uint enable  = 0;
    uint expire  = 0;
};
