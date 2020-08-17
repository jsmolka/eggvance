#pragma once

#include "arm/arm.h"
#include "core/context.h"
#include "dma/dmac.h"
#include "irq/irqh.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timerc.h"

class Core
{
public:
    Core();

    void init(int argc, char* argv[]);

    void reset();
    void frame();

    void updateWindowTitle();
    void updateWindowTitle(double fps);

    MMU mmu;
    ARM arm;
    PPU ppu;
    Keypad keypad;
    IrqHandler irqh;
    DmaController dmac;
    TimerController timerc;
    Context context;
};
