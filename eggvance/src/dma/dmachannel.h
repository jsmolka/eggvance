#pragma once

#include "io.h"
#include "arm/constants.h"

class DmaChannel
{
public:
    friend class Dma;

    DmaChannel(uint id);

    void reload();
    bool start();
    void run();

    const uint id;
    DmaCount count;
    DmaControl control;
    RegisterW<u32> sad;
    RegisterW<u32> dad;

private:
    void initTransfer();
    void initEeprom();

    int running = false;
    int fifo    = false;
    int pending = 0;
    u32 bus     = 0;

    struct Internal
    {
        u32 count    = 0;
        u32 src_addr = 0;
        u32 dst_addr = 0;
    } internal;

    std::function<void(Access)> transfer;
};
