#pragma once

#include "common/macros.h"
#include "common/utility.h"

class DMAAddr
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    u32 addr;
    u32 addr_mask;

private:
    void update();
};

#include "dmaaddr.inl"
