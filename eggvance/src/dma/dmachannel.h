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

    bool isFifoA() const;
    bool isFifoB() const;

    const uint id;
    bool running = false;

    DmaCount count;
    DmaControl control;
    RegisterW<u32> sad;
    RegisterW<u32> dad;
    bool fifo = false;

private:
    void initTransfer();
    void initEeprom();

    int pending = 0;
    u32 bus     = 0;

    struct
    {
        u32 count    = 0;
        u32 src_addr = 0;
        u32 dst_addr = 0;
    } internal;

    std::function<void(Access)> transfer;
};
