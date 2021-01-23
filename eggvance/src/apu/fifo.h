#pragma once

#include <shell/ringbuffer.h>

#include "base/int.h"

class Fifo : public shell::RingBuffer<u8, 32>
{
public:
    void tick();
    bool refillable() const;

    s16 sample = 0;
};
