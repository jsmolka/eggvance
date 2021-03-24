#pragma once

#include <array>

#include "base/int.h"

class WaveRam
{
public:
    u8 operator[](uint index) const;

    u8 read(uint index) const;
    void write(uint index, u8 byte);

    uint bank = 0;

private:
    std::array<u8, 32> ram = {};
};
