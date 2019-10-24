#pragma once

#include "common/macros.h"
#include "common/utility.h"

class Mosaic
{
public:
    struct Stretch
    {
        void reset();

        int x;
        int y;
    };

    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    Stretch bgs;
    Stretch obj;
};

#include "mosaic.inl"
