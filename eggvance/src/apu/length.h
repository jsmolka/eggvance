#pragma once

#include "base/int.h"

template<uint Base>
class Length
{
public:
    Length& operator=(uint value)
    {
        length = value;

        init();

        return *this;
    }

    void init()
    {
        timer = Base - length;
    }

    void tick()
    {
        if (timer && --timer == 0 && !expire)
            init();
    }

    uint enabled() const
    {
        return timer > 0;
    }

    uint timer  = 0;
    uint length = 0;
    uint expire = 0;
};
