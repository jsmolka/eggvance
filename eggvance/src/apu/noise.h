#pragma once

#include <algorithm>
#include <cmath>

#include "channel.h"
#include "envelope.h"
#include "length.h"

class Noise : public Channel
{
public:
    Noise();

    void init();
    void tick();
    void tickLength();
    void tickEnvelope();
    void write(std::size_t index, u8 byte);

private:
    enum NR { k41 = 0, k42 = 1, k43 = 4, k44 = 5 };

    void updateTimer();

    Length<64> length;
    Envelope envelope;

    uint shift     = 0;
    uint frequency = 1;
    uint noise     = 0;
};
