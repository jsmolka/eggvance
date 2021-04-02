#pragma once

#include <shell/array.h>

#include "base/int.h"

class WaveRam
{
public:
    u8 operator[](uint index) const;

    u8 read(uint index) const;
    void write(uint index, u8 byte);

    uint bank = 0;

private:
    union
    {
        shell::array<u8, 32> data = {};
        shell::array<u8, 2, 16> banks;
    };
};
