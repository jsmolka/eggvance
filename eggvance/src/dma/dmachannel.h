#pragma once

#include "dma/io.h"

class DmaChannel
{
public:
    DmaChannel(uint id)
        : id(id) {}

    void start();
    void run(int& cycles);

    uint id{};
    bool running{};

    DmaCount count;
    DmaControl control;
    RegisterW<4> sad;
    RegisterW<4> dad;

private:
    static bool isEeprom(u32 addr);
    static bool isGamePak(u32 addr);

    void initCycles();
    void initTransfer();
    void initEeprom();

    int pending{};
    int cycles_s{};
    int cycles_n{};
    u32 sad_value{};
    u32 dad_value{};

    std::function<void(void)> transfer;
};
