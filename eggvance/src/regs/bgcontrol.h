#pragma once

#include "register.h"

class BGControl : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

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
