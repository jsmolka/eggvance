#include "ppu.h"

#include "mmu/map.h"
#include "tiles/mapentry.h"

void PPU::renderText()
{
    const Bgcnt& bgcnt = mmu.bg0cnt;

    u32 mapAddr = 0x0600FE1C;  // bgcnt.mapAddr();

    MapEntry entry(mmu.readHalf(mapAddr));

    u32 addr = bgcnt.tileAddr() + 0x20  * entry.tile;

    // Iterate vertical tile pixels
    for (int y = 0; y < 8; ++y)
    {
        // Iterate horizontal tile pixels (2 pixels per byte)
        for (int x = 0; x < 4; x++)
        {
            int byte = mmu.readByteFast(addr);

            draw(2 * x, y, readBgColor(byte & 0xF, entry.palette));
            draw(2 * x + 1, y, readBgColor(byte >> 4, entry.palette));

            addr++;
        }
    }
}
