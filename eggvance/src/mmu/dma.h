#pragma once

#include "common/integer.h"

class MMU;

enum DMATiming
{
    DT_NOW     = 0,
    DT_VBLANK  = 1,
    DT_HBLANK  = 2,
    DT_REFRESH = 3
};

class DMA
{
public:
    DMA(int id, MMU& mmu);

    void reset();
    void run(DMATiming timing);

    union
    {
        u8  addr_b[4];  // Address bytes
        u32 addr;       // Address
    } src, dst;

    union
    {
        u8  units_b[2];  // Units bytes
        u16 units;       // Units
    };

    struct Control
    {
        int src_control;  // Destination address control
        int dst_control;  // Source address control
        int repeat;       // DMA repeat
        int word;         // DMA transfer type
        int gamepak_drq;  // ???
        int timing;       // Start timing
        int irq;          // IRQ on end of word count
        int enable;       // DMA enable
    } control; 

private:
    enum DestinationAdjustment
    {
        DA_INC = 0,
        DA_DEC = 1,
        DA_FIX = 2,
        DA_RLD = 3
    };
    enum SourceAdjustment
    {
        SA_INC = 0,
        SA_DEC = 1,
        SA_FIX = 2
    };

    int id;
    MMU& mmu;
};
