#pragma once

#include "regs/dmaaddress.h"
#include "regs/dmacontrol.h"
#include "regs/dmacount.h"

class DMA
{
public:
    enum class Timing
    {
        Immediate = 0,
        VBlank    = 1,
        HBlank    = 2,
        Special   = 3
    };

    DMA(int id);

    void reset();

    void start();
    void run(int& cycles);

    int id;
    bool running;
    DMACount count;
    DMAAddress sad;
    DMAAddress dad;
    DMAControl control;

private:
    using TransferFunc = void(DMA::*)(void);

    static bool inEEPROM(u32 addr);
    static bool inGamePak(u32 addr);

    void updateCycles();
    void updateTransfer();

    void transferHalf();
    void transferWord();

    void initEEPROM();
    void readEEPROM();
    void writeEEPROM();

    int remaining;
    u32 sad_addr;
    u32 dad_addr;
    int sad_delta;
    int dad_delta;
    int cycles_s;
    int cycles_n;

    TransferFunc transfer;
};
