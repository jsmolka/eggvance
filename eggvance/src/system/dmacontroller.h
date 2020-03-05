#pragma once

#include "dma.h"

class DMAController
{
public:
    void reset();

    void run(int& cycles);
    void broadcast(DMA::Timing timing);

    u8 read(u32 addr) const;
    void write(u32 addr, u8 byte);

private:
    void emit(DMA& dma, DMA::Timing timing);

    template<uint index>
    void writeControl(DMA& dma, u8 byte);

    DMA* active;
    DMA dmas[4] = { 0, 1, 2, 3 };
};

extern DMAController dmac;
