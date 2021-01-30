#pragma once

#include "channel.h"

class Noise : public Channel
{
public:
    Noise();

    void init();
    void tick();
    void write(uint index, u8 byte);

protected:
    uint period() const final;

private:
    enum NR { k41 = 0, k42 = 1, k43 = 4, k44 = 5 };

    uint shift = 0;
    uint noise = 0;
};
