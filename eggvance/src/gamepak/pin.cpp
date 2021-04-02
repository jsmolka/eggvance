#include "pin.h"

#include <shell/macros.h>

Pin& Pin::operator=(uint value)
{
    SHELL_ASSERT(value == 0 || value == 1);

    prev = data;
    data = value;

    return *this;
}

Pin::operator uint() const
{
    return data;
}

bool Pin::low() const
{
    return data == 0;
}

bool Pin::high() const
{
    return data == 1;
}

bool Pin::rising() const
{
    return prev == 0 && data == 1;
}

bool Pin::falling() const
{
    return prev == 1 && data == 0;
}
