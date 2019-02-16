#include "ppu.h"

#include "common/memory_map.h"

void PPU::renderText()
{
    const auto& bgcnt = mmu->bgcnt[0];

    for (int y = 0; y < 20; ++y)
    {
        for (int x = 0; x < 30; ++x)
        {
            // Each tile takes up 32 words in memory
            u16 tile = mmu->readHalf(bgcnt.map_addr + 32 * 2 * y + 2 * x);

            u16 tile_number = tile & 0x3FF;
            u8 palette_number = tile >> 12 & 0xF;

            int x_off = 0;
            int y_off = 0;

            // 8x8 tiles at address of tile number
            for (int i = 0; i < 32; ++i)
            {
                u8 tile_data = mmu->readByte(bgcnt.data_addr + 32 * tile_number + i);

                if (x_off == 8)
                {
                    x_off = 0;
                    y_off++;
                }

                u16 color_lhs = mmu->readHalf(MAP_PALETTE + 32 * palette_number + 2 * (tile_data & 0xF));
                u16 color_rhs = mmu->readHalf(MAP_PALETTE + 32 * palette_number + 2 * (tile_data >> 4 & 0xF));

                drawPixel(8 * x + x_off, 8 * y + y_off, color_lhs);
                drawPixel(8 * x + x_off + 1, 8 * y + y_off, color_rhs);

                x_off += 2;
            }
        }
    }
}