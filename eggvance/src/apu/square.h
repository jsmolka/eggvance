#pragma once

#include "channel.h"

class Square : public Channel
{
public:
    Square(u64 mask);

    void tick();

protected:
    void init();

    uint period() const final;

    uint step = 0;
    uint form = 0;
};
