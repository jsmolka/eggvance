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

    int sad_ctrl;
    int dad_ctrl;
    int repeat;
    int word;
    int drq;
    int timing;
    int irq;
    int enable;
    int count;
    int count_mask;

    bool reload;
};

#include "dmacnt.inl"
