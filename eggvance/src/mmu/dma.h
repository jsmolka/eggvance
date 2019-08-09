#pragma once

#include "common/integer.h"

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

    struct Control
    {
        int src_adjust;   // Destination address control
        int dst_adjust;   // Source address control
        int repeat;       // DMA repeat
        int word;         // DMA transfer type
        int gamepak_drq;  // ???
        int timing;       // Start timing
        int irq;          // IRQ on end of word count
        int enable;       // DMA enable
    } control; 

    union
    {
        u8  addr_b[4];  // Address bytes
        u32 addr;       // Address
    } src, dst;

    union
    {
        u8  count_b[2];  // Word count bytes
        u16 count;       // Word count
    };

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
