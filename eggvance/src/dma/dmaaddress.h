#pragma once

#include "base/int.h"

class DmaAddress
{
public:
    DmaAddress& operator=(u32 value);
    operator u32() const;

    bool isFifoA() const;
    bool isFifoB() const;
    bool isGamePak() const;

private:
    u32 data = 0;
};
