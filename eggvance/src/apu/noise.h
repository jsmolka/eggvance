#pragma once

#include "channel.h"

class Noise final : public Channel
{
public:
    Noise();

    void tick();
    void write(uint index, u8 byte);

protected:
    void init();

    uint period() const final;

private:
    uint noise  = 0;
    uint ratio  = 0;
    uint narrow = 0;
    uint shift  = 0;
};
