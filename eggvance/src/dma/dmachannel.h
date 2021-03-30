#pragma once

#include <functional>

#include "dmaaddress.h"
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
    DmaSrcAddress sad;
    DmaDstAddress dad;
    DmaCount count;
    DmaControl control;

private:
    void initEeprom();
    void initTransfer();

    uint running = 0;
    uint pending = 0;
    uint bus     = 0;

    struct
    {
        uint fifo   = 0;
        uint sadcnt = 0;
        uint dadcnt = 0;
        uint word   = 0;
        uint count  = 0;
        DmaAddress sad;
        DmaAddress dad;
    } latch;

    std::function<void(Access)> transfer;
};
