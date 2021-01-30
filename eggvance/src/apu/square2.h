#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"

class Square2 : public Channel
{
public:
    Square2();

    void init();
    void tick();
    void tickLength();
    void tickEnvelope();

    void write(std::size_t index, u8 byte);

private:
    enum NR { k21 = 0, k22 = 1, k23 = 4, k24 = 5 };

    void updateTimer();

    Length<64> length;
    Envelope envelope;

    uint step      = 0;
    uint pattern   = 0;
    uint frequency = 0;
};
