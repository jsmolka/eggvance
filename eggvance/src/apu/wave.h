#pragma once

#include "channel.h"
#include "waveram.h"

class Wave : public Channel
{
public:
    Wave();

    void init();
    void tick();
    void write(uint index, u8 byte);

    WaveRam ram;

protected:
    uint period() const final;

private:
    enum NR { k30 = 0, k31 = 2, k32 = 3, k33 = 4, k34 = 5 };

    uint position  = 0;
    uint dimension = 0;
    uint active    = 0;
    uint volume    = 0;
};
