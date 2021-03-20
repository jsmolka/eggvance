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
    DmaSource sad;
    DmaDestination dad;
    DmaCount count;
    DmaControl control;

private:
    void initTransfer();
    void initEeprom();

    uint running = false;
    uint fifo    = false;
    uint pending = 0;
    uint bus     = 0;

    struct Internal
    {
        uint sad   = 0;
        uint dad   = 0;
        uint count = 0;
    } internal;

    std::function<void(Access)> transfer;
};
