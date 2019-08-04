#pragma once

#include <string>
#include <SDL2/SDL_events.h>

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"

class Core
{
public:
    Core();

    void run(const std::string& file);

private:
    void frame();
    void emulate(int cycles);
    void emulateTimers(int cycles);

    void keyEvent(const SDL_KeyboardEvent& event);

    MMU mmu;
    PPU ppu;
    ARM arm;
};
