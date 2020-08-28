#pragma once

#include <functional>

#include "dma/io.h"

class Dma
{
public:
    enum Timing
    {
        kTimingImmediate,
        kTimingVBlank,
        kTimingHBlank,
        kTimingSpecial
    };

    Dma(uint id);

    void activate();
    void run(int& cycles);

    uint id{};
    bool running{};
    DmaIo io;

private:
    bool isEeprom(u32 addr);
    bool isGamePak(u32 addr);

    void initCycles();
    void initTransfer();
    void initEeprom();

    int pending{};
    int cycles_s{};
    int cycles_n{};
    u32 sad{};
    u32 dad{};
    int sad_delta{};
    int dad_delta{};

    std::function<void(void)> transfer;
};
