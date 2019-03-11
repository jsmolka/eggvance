#include "registers.h"

#include "common/memory_map.h"

void DisplayControl::write(u8 pos, u8 byte)
{
    switch (pos)
    {
    case 0:
        bg_mode = byte & 0x7;
        gbc_mode = byte >> 3 & 0x1;
        frame_select = (byte >> 4 & 0x1) ? MAP_VRAM : (MAP_VRAM + 0xA000);
        process_hblank = byte >> 5 & 0x1;
        obj_mapping = byte >> 6 & 0x1;
        forced_blank = byte >> 7 & 0x1;
        break;

    case 1:
        bg0_enable = byte & 0x1;
        bg1_enable = byte >> 1 & 0x1;
        bg2_enable = byte >> 2 & 0x1;
        bg3_enable = byte >> 3 & 0x1;
        oam_enable = byte >> 4 & 0x1;
        win0_enable = byte >> 5 & 0x1;
        win1_enable = byte >> 6 & 0x1;
        obj_win_enable = byte >> 7 & 0x1;
        break;
    }
}

void DisplayStatus::write(u8 pos, u8 byte)
{
    switch (pos)
    {
    case 0:
        vblank_flag = byte & 0x1;
        hblank_flag = byte >> 1 & 0x1;
        vcount_flag = byte >> 2 & 0x1;
        vblank_irq_enable = byte >> 3 & 0x1;
        hblank_irq_enable = byte >> 4 & 0x1;
        vcount_irq_enable = byte >> 5 & 0x1;
        break;

    case 1:
        vcount_trigger = byte;
        break;
    }
}

void BackgroundControl::write(u8 pos, u8 byte)
{
    switch (pos)
    {
    case 0:
        priority = byte & 0x3;
        data_addr = MAP_VRAM + 0x4000 * (byte >> 2 & 0x3);
        mosaic = byte >> 6 & 0x1;
        palette = byte >> 7 & 0x1;
        break;

    case 1:
        map_addr = MAP_VRAM + 0x800 * (byte & 0x1F);
        disp_overflow = byte >> 5 & 0x1;
        map_size = byte >> 6 & 0x3;
        break;
    }
}
