#pragma once

#include "channel.h"

class Square : public Channel
{
public:
    Square(u64 mask);

    void init();
    void tick();

protected:
    uint period() const final;

    uint step    = 0;
    uint pattern = 0;
};
