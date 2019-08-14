#pragma once

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "input.h"

class Core
{
public:
    Core(std::shared_ptr<BIOS> bios);

    void run(std::shared_ptr<GamePak> gamepak);

private:
    void reset();

    void drawIcon();
    bool dropAwait();
    bool dropEvent(const SDL_DropEvent& event);

    void frame();
    void emulate(int cycles);
    void emulateTimers(int cycles);

    MMU mmu;
    PPU ppu;
    ARM arm;
    Input input;
};
