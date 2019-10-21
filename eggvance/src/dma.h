#pragma once

#include "mmu/registers/dmaaddr.h"
#include "mmu/registers/dmacnt.h"

class DMA
{
public:
    enum class Timing
    {
        Immediate = 0,
        VBlank    = 1,
        HBlank    = 2,
        Refresh   = 3
    };

    enum class State
    {
        Finished,
        Running
    } state;

    void reset();
    void start();

    void run(int& cycles);

    int id;
    int count;
    int sad_delta;
    int dad_delta;
    u32 sad_addr;
    u32 dad_addr;

    DMAAddr sad;
    DMAAddr dad;
    DMACnt control;

private:
    static int deltas[4];
};
