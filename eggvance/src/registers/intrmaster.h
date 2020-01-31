#pragma once

#include "common/integer.h"

class IntrMaster
{
public:
    IntrMaster();

    void reset();

    template<uint index>
    inline u8 read() const
    {
        static_assert(index <= 1);

        return data[index];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index <= 1);

        if (index == 0)
            master = byte & 0x1;

        data[index] = byte;
    }

    inline operator bool() const
    {
        return master;
    }

    bool master;

private:
    u8 data[2];
};
