#pragma once

#include "common/macros.h"
#include "common/utility.h"

class BldAlpha
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int eva;
    int evb;

private:
    struct Register
    {
        int eva;
        int evb;
    } reg;

    void update();
};

#include "bldalpha.inl"
