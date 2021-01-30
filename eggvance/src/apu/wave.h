#pragma once

#include "channel.h"
#include "length.h"
#include "waveram.h"

class Wave : public Channel
{
public:
    Wave();

    void init();
    void tick();
    void tickLength();

    void write(std::size_t index, u8 byte);

    WaveRam ram;

private:
    enum NR { k30 = 0, k31 = 2, k32 = 3, k33 = 4, k34 = 5 };

    void updateTimer();

    Length<256> length;

    uint position  = 0;
    uint dimension = 0;
    uint active    = 0;
    uint volume    = 0;
    uint frequency = 0;
};
