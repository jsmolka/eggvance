#pragma once

#include "io.h"

class DmaChannel
{
public:
    DmaChannel(uint id);

    void start();
    void run(int& cycles);

    const uint id;
    bool running = false;

    DmaCount count;
    DmaControl control;
    RegisterW<u32> sad;
    RegisterW<u32> dad;

private:
    static bool isGamePak(u32 addr);

    void initCycles();
    void initTransfer();
    void initEeprom();

    int pending  = 0;
    int cycles_s = 0;
    int cycles_n = 0;
    u32 src_addr = 0;
    u32 dst_addr = 0;

    std::function<void(void)> transfer;
};
