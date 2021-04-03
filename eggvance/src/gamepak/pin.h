#pragma once

#include "base/int.h"

class Pin
{
public:
    Pin& operator=(uint value);
    operator uint() const;

    bool isLow() const;
    bool isHigh() const;
    bool isRising() const;
    bool isFalling() const;

private:
    uint data = 0;
    uint prev = 0;
};
