#pragma once

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "input.h"

class Core
{
public:
    Core(std::unique_ptr<BIOS> bios);

    void run(std::unique_ptr<GamePak> gamepak);

private:
    void reset();

    void drawIcon();
    bool dropAwait();
    bool dropEvent(const SDL_DropEvent& event);

    void coreKeyEvent(const SDL_KeyboardEvent& event);
    void setWindowTitle(const GamePak& gamepak);

    void frame();
    void emulate(int cycles);
    void emulateTimers(int cycles);

    int remaining;
    bool limited;

    std::string window_title;

    MMU mmu;
    PPU ppu;
    ARM arm;
    Input input;
};
