#pragma once

#include "channel.h"
#include "envelope.h"
#include "sweep.h"

class Square1 : public Channel<0x007F, 0xFFC0, 0x4000>
{
public:
    void tick();

    Envelope envelope;
    Sweep sweep;
};
