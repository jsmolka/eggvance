#include "pin.h"

#include <shell/macros.h>

Pin& Pin::operator=(uint value)
{
    prev = data;
    data = value & 0x1;

    return *this;
}

Pin::operator uint() const
{
    return data;
}

bool Pin::isLow() const
{
    return data == 0;
}

bool Pin::isHigh() const
{
    return data == 1;
}

bool Pin::isRising() const
{
    return prev == 0 && data == 1;
}

bool Pin::isFalling() const
{
    return prev == 1 && data == 0;
}
