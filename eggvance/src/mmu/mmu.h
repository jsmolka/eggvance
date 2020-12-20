#pragma once

#include "bios.h"
#include "mmio.h"
#include "oam.h"
#include "paletteram.h"
#include "videoram.h"

class Mmu
{
public:
    PaletteRam pram;
    VideoRam vram;
    Oam oam;
    Bios bios;
    Ram<0x40000> ewram;
    Ram<0x08000> iwram;
    Mmio mmio;
};

inline Mmu mmu;
