#pragma once

#include "common/macros.h"
#include "common/utility.h"

class IntEnabled
{
public:
    void reset();

    inline operator int() const;

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int value;
};

#include "intenabled.inl"
