#pragma once

#include "base/int.h"

class WaveRam
{
public:
    u8 operator[](uint index) const;

    u8 read(uint index);
    void write(uint index, u8 byte);

    uint bank = 0;

private:
    u8 data[32] = {};
};
