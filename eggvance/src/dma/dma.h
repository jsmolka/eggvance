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
    void run(int& cycles);

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

    int remaining = 0;
    int cycles_s  = 0;
    int cycles_n  = 0;
    u32 sad       = 0;
    u32 dad       = 0;
    int sad_delta = 0;
    int dad_delta = 0;
    Transfer transfer = nullptr;
};
