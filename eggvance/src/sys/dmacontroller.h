#pragma once

#include "dma.h"

class DMAController
{
public:
    DMAController();

    void reset();

    void run(int& cycles);
    void signal(DMA::Timing timing);

    u8 readByte(u32 addr);
    void writeByte(u32 addr, u8 byte);

    DMA* active;
    DMA dmas[4];
};
