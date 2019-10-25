#pragma once

#include "regs/dmaaddress.h"
#include "regs/dmacontrol.h"

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

    DMAAddress sad;
    DMAAddress dad;
    DMAControl control;

private:
    bool readEEPROM(int& cycles);
    bool writeEEPROM(int& cycles);

    static int deltas[4];
};
