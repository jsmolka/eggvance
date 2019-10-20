#pragma once

#include "common/macros.h"
#include "common/integer.h"

class BgCnt
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int size() const;
    int width() const;
    int height() const;

    int priority;
    int tile_block;
    int mosaic;
    int pformat;
    int map_block;
    int wraparound;
    int screen_size;

private:
    static int sizes[4][2];
};

#include "bgcnt.inl"
