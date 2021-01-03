#pragma once

#include "io.h"
#include "arm/constants.h"

class DmaChannel
{
public:
    DmaChannel(uint id);

    void reload();
    void start();
    void run();

    uint id;
    bool running = false;
    uint fifo    = false;

    DmaCount count;
    DmaControl control;
    RegisterW<u32> sad;
    RegisterW<u32> dad;

private:
    void initTransfer();
    void initEeprom();

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
