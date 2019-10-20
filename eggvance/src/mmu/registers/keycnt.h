#pragma once

#include "common/macros.h"
#include "common/utility.h"

class KeyCnt
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int keys;
    int irq;
    int irq_logic;
};

#include "keycnt.inl"