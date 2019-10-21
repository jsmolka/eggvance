#pragma once

#include "common/macros.h"
#include "common/utility.h"

class IntMaster
{
public:
    void reset();

    inline operator bool() const;

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int enabled;
};

#include "intmaster.inl"