#pragma once

#include "registers/dmaaddress.h"
#include "registers/dmacontrol.h"

class MMU;

class DMA
{
public:
    enum Timing
    {
        IMMEDIATE = 0,
        VBLANK    = 1,
        HBLANK    = 2,
        REFRESH   = 3
    };

    DMA(int id, MMU& mmu);

    void reset();

    void activate();
    bool emulate(int& cycles);

    DMAControl& control;
    DMAAddress& sad;
    DMAAddress& dad;

    int id;
    bool active;

private:
    enum Adjustment
    {
        ADJ_INCREMENT = 0,
        ADJ_DECREMENT = 1,
        ADJ_FIXED     = 2,
        ADJ_RELOAD    = 3
    };

    int stepDifference(Adjustment adj);

    void reload();
    void interrupt();

    int remaining;
    int seq_cycles;
    u32 addr_dst;
    int diff_dst;
    int diff_src;
    MMU& mmu;
};
