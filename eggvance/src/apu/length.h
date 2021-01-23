#pragma once

#include "base/int.h"

template<uint Base>
class Length
{
public:
    void init();
    void tick();
    uint enabled() const;

    uint length = 0;
    uint expire = 0;
    uint timer  = 0;
};

template<uint Base>
void Length<Base>::init()
{
    timer = Base - length;
}

template<uint Base>
void Length<Base>::tick()
{
    if (timer && --timer == 0 && !expire)
        init();
}

template<uint Base>
uint Length<Base>::enabled() const
{
    return timer > 0;
}
