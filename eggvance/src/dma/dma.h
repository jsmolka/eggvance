#pragma once

#include "io/dmaio.h"

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

    DMA(uint id);

    void start();
    void run(cycle_t& cycles);

    uint id      = 0;
    bool running = false;
    DMAIO io;

private:
    using Transfer = void(DMA::*)(void);

    static bool inEEPROM(u32 addr);
    static bool inGamePak(u32 addr);

    void updateCycles();
    void updateTransfer();

    void transferHalf();
    void transferWord();

    void initEEPROM();
    void readEEPROM();
    void writeEEPROM();

    cycle_t remaining = 0;
    cycle_t cycles_s  = 0;
    cycle_t cycles_n  = 0;
    u32 sad           = 0;
    u32 dad           = 0;
    int sad_delta     = 0;
    int dad_delta     = 0;
    Transfer transfer = nullptr;
};
