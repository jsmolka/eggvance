#pragma once

#include "channel.h"
#include "waveram.h"

class Wave final : public Channel
{
public:
    Wave();

    void tick();
    void write(uint index, u8 byte);

    WaveRam ram;

protected:
    void init();
    
    uint period() const final;

private:
    uint step   = 0;
    uint wide   = 0;
    uint active = 0;
    uint volume = 0;
};
