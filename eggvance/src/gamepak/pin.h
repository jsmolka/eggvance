#pragma once

#include "base/int.h"

class Pin
{
public:
    Pin& operator=(uint value);
    operator uint() const;

    bool low() const;
    bool high() const;
    bool rising() const;
    bool falling() const;

private:
    uint data = 0;
    uint prev = 0;
};
