#pragma once

#include "cartridge/cartridge.h"
#include "ram.h"

struct Memory
{
    Cartridge cartridge;

    RAM<0x40000> ewram;
    RAM<0x08000> iwram;
    RAM<0x00400> io;
    RAM<0x00400> pram;
    RAM<0x18000> vram;
    RAM<0x00400> oam;
};
