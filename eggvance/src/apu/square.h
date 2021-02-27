#pragma once

#include "channel.h"

class Square : public Channel
{
public:
    Square(u64 mask);

    void tick() final;

protected:
    uint period() const final;
    
    void init();

    uint step = 0;
    uint form = 0;
};
