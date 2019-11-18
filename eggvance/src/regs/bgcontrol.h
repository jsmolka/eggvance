#pragma once

#include "register.h"

class BGControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int size() const;
    int width() const;
    int height() const;

    u32 mapBase() const;
    u32 tileBase() const;

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
