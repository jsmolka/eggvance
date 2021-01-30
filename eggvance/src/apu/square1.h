#pragma once

#include "channel.h"
#include "envelope.h"
#include "length.h"
#include "sweep.h"

class Square1 : public Channel
{
public:
    Square1();

    void init();
    void tick();
    void tickSweep();
    void tickLength();
    void tickEnvelope();

    void write(std::size_t index, u8 byte);

private:
    enum NR { k10 = 0, k11 = 2, k12 = 3, k13 = 4, k14 = 5 };

    void updateTimer();
    void updateSweep(bool writeback);

    Length<64> length;
    Envelope envelope;
    Sweep sweep;

    uint step      = 0;
    uint pattern   = 0;
    uint frequency = 0;
};
