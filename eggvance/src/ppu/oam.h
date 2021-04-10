#pragma once

#include <shell/array.h>

#include "matrix.h"
#include "oamentry.h"
#include "base/ram.h"

class Oam : public Ram<1024>
{
public:
    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    shell::array<OamEntry, 128> entries = {};
    shell::array<RotationScalingMatrix, 32> matrices = {};
};
