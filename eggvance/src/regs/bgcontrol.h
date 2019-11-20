#pragma once

#include "ppu/dimensions.h"
#include "register.h"

class BGControl : public Register<2>
{
public:
    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    u32 mapBase() const;
    u32 tileBase() const;

    int priority;
    int tile_block;
    int mosaic;
    int pformat;
    int map_block;
    int wraparound;
    int screen_size;

    Dimensions dims_aff;
    Dimensions dims_reg;

private:
    void updateDims();
};
