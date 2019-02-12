#pragma once

#include "integer.h"

struct lcd_stat
{
    u16 display_control;
    u16 display_stat;
    u16 vcount;

    u8 bg_priority[4];
    u32 bg_tile_data_addr[4];
    bool bg_mosaic[4];
    u8 bg_palette_type[4];
    u32 bg_tile_map_addr[4];
    u8 bg_tile_map_size[4];
};