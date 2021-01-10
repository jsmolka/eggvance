#pragma once

#include "channel.h"
#include "envelope.h"

class Noise : public Channel<0xFF00, 0x0000, 0x40FF>
{
public:
    void tick();

    Envelope envelope;
};
