#pragma once

#include "ppu/dimensions.h"
#include "register.h"

class BGControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int priority;
    int tile_block;
    int mosaic;
    int color_mode;
    int map_block;
    int wraparound;
    int screen_size;

    Dimensions dims_aff;
    Dimensions dims_reg;

    u32 base_map;
    u32 base_tile;

private:
    void update();
};
