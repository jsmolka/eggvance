#pragma once

#include <string>

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "input.h"

class Core
{
public:
    Core();

    void run(const std::string& file);

private:
    void frame();
    void emulate(int cycles);
    void emulateTimers(int cycles);

    MMU mmu;
    PPU ppu;
    ARM arm;
    Input input;
};
