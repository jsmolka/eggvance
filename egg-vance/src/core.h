#pragma once

/**
 * Todo:
 * - execute ARM instructions until one frame cane be drawn
 */

#include <string>

#include "arm/arm.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"

class Core
{
public:
    Core();

    void run(const std::string& file);

private:
    ARM arm;
    MMU mmu;
    PPU ppu;
};

