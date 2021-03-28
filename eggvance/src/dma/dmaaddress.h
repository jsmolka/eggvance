#pragma once

#include "base/int.h"

class DmaAddress
{
public:
    DmaAddress& operator=(uint value);
    operator uint() const;

    bool fifoA() const;
    bool fifoB() const;
    bool gamepak() const;

private:
    uint data = 0;
};
