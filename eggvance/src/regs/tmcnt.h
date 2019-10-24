#pragma once

#include "common/macros.h"
#include "common/utility.h"

class TmCnt
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int prescaler;
    int cascade;
    int irq;
    int enabled;

    u16 data;
    u16 initial;
};

#include "tmcnt.inl"
