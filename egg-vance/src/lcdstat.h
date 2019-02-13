#pragma once

#include "integer.h"

struct LcdStat
{
    u16 display_control;
    u16 display_stat;
    u16 vcount;

    u8 bg_priority[4];
    u32 bg_data_addr[4];
    bool bg_mosaic[4];
    u8 bg_palette_type[4];
    u32 bg_map_addr[4];
    u8 bg_map_size[4];
};
