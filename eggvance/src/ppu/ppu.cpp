#include "ppu.h"

PPU ppu;

void PPU::reset()
{
    oam.reset();
    palette.reset();
    vram.reset();
}
