#pragma once

#include <functional>

#include "dma/io.h"

class Dma
{
public:
    enum Timing
    {
        kTimingImmediate = 0,
        kTimingVBlank    = 1,
        kTimingHBlank    = 2,
        kTimingSpecial   = 3
    };

    Dma(uint id);

    void start();
    void run(int& cycles);

    uint id      = 0;
    bool running = false;
    DmaIo io;

private:
    static bool inEEPROM(u32 addr);
    static bool inGamePak(u32 addr);

    void updateCycles();
    void updateTransfer();

    void initEEPROM();

    int remaining = 0;
    int cycles_s  = 0;
    int cycles_n  = 0;
    u32 sad       = 0;
    u32 dad       = 0;
    int sad_delta = 0;
    int dad_delta = 0;

    std::function<void(void)> transfer;
};
