#pragma once

#include "channel.h"
#include "waveram.h"

class Wave final : public Channel
{
public:
    Wave();

    void tick() final;
    void write(uint index, u8 byte);

    WaveRam ram;

protected:
    uint period() const final;
    
    void init();

private:
    void setStep(uint value);

    uint step   = 0;
    uint wide   = 0;
    uint active = 0;
    uint volume = 0;
};
