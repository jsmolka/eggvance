#pragma once

#include "common/macros.h"
#include "common/utility.h"

class DispStat
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int vblank;
    int hblank;
    int vmatch;
    int vblank_irq;
    int hblank_irq;
    int vmatch_irq;
    int vcompare;
};

#include "dispstat.inl"
