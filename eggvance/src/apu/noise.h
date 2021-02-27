#pragma once

#include "channel.h"

class Noise final : public Channel
{
public:
    Noise();

    void tick() final;
    void write(uint index, u8 byte);

protected:
    uint period() const final;

    void init();

private:
    uint noise  = 0;
    uint ratio  = 0;
    uint narrow = 0;
    uint shift  = 0;
};
