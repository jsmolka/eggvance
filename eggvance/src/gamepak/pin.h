#pragma once

#include "base/int.h"

class Pin
{
public:
    Pin& operator=(uint data)
    {
        this->prev = this->data;
        this->data = data;

        return *this;
    }

    operator uint() const
    {
        return data;
    }

    bool low()     const { return data == 0; }
    bool high()    const { return data == 1; }
    bool rising()  const { return prev == 0 && data == 1; }
    bool falling() const { return prev == 1 && data == 0; }

    uint data = 0;
    uint prev = 0;
};
