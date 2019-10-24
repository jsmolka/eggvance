#pragma once

#include "common/macros.h"
#include "common/utility.h"

class DMACnt
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int sad_delta;
    int dad_delta;
    int repeat;
    int word;
    int drq;
    int timing;
    int irq;
    int enabled;
    int count;
    int count_mask;

    bool update;
};

#include "dmacnt.inl"
