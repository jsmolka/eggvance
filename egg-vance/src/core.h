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

    void frame();
    void runCycles(int cycles);

    void processKeyEvent(SDL_Keycode key, bool down);

    MMU mmu;
    PPU ppu;
    ARM arm;
};

