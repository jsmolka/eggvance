#pragma once

#include "common/macros.h"
#include "common/utility.h"

class BldY
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int evy;

private:
    struct Register
    {
        int evy;
    } reg;

    void update();
};

#include "bldy.inl"
