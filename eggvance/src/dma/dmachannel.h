#pragma once

#include <functional>

#include "io.h"
#include "arm/enums.h"

class DmaChannel
{
public:
    friend class Dma;

    DmaChannel(uint id);

    void init();
    bool start();
    void run();

    const uint id;
    DmaSource sad;
    DmaDestination dad;
    DmaCount count;
    DmaControl control;

private:
    void initEeprom();
    void initTransfer();

    uint running = 0;
    uint fifo    = 0;
    uint pending = 0;
    uint bus     = 0;

    struct Latch
    {
        uint sad   = 0;
        uint dad   = 0;
        uint count = 0;
    } latch;

    std::function<void(Access)> transfer;
};
