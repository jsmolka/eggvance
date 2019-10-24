#pragma once

#include "common/macros.h"
#include "common/utility.h"

class VCount
{
public:
    void reset();

    inline operator int() const;
    inline VCount& operator=(int value);

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int line;
};

#include "vcount.inl"
