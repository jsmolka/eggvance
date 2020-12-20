#pragma once

#include "bios.h"
#include "mmio.h"

class Mmu
{
public:
    Bios bios;
    Ram<0x40000> ewram;
    Ram<0x08000> iwram;
    Mmio mmio;
};

inline Mmu mmu;
