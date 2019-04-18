#pragma once

#include <string>

#include <SDL_events.h>

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"

class Core
{
public:
    Core();

    void run(const std::string& file);

private:
    void reset();

    u16 keyMask(const SDL_Keycode& key);

    MMU mmu;
    PPU ppu;
    ARM arm;
};

