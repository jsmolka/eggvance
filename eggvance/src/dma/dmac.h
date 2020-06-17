#pragma once

#include "base/constants.h"
#include "base/macros.h"
#include "dma/dma.h"

class DMAController
{
public:
    friend class Io;

    void run(int& cycles);
    void broadcast(DMA::Timing timing);

private:
    template<uint addr>
    u8 read() const;

    template<uint addr>
    void write(u8 byte);
    
    void emit(DMA& dma, DMA::Timing timing);

    DMA* active = nullptr;
    DMA dmas[4] = { 0, 1, 2, 3 };
};

extern DMAController dmac;

template<uint addr>
u8 DMAController::read() const
{    
    switch (addr)
    {
    CASE4(kRegDma0Sad    , return dmas[0].io.sad.read<kIndex>())
    CASE4(kRegDma1Sad    , return dmas[1].io.sad.read<kIndex>())
    CASE4(kRegDma2Sad    , return dmas[2].io.sad.read<kIndex>())
    CASE4(kRegDma3Sad    , return dmas[3].io.sad.read<kIndex>())
    CASE4(kRegDma0Dad    , return dmas[0].io.dad.read<kIndex>())
    CASE4(kRegDma1Dad    , return dmas[1].io.dad.read<kIndex>())
    CASE4(kRegDma2Dad    , return dmas[2].io.dad.read<kIndex>())
    CASE4(kRegDma3Dad    , return dmas[3].io.dad.read<kIndex>())
    CASE2(kRegDma0Count  , return dmas[0].io.count.read<kIndex>())
    CASE2(kRegDma1Count  , return dmas[1].io.count.read<kIndex>())
    CASE2(kRegDma2Count  , return dmas[2].io.count.read<kIndex>())
    CASE2(kRegDma3Count  , return dmas[3].io.count.read<kIndex>())
    CASE2(kRegDma0Control, return dmas[0].io.control.read<kIndex>())
    CASE2(kRegDma1Control, return dmas[1].io.control.read<kIndex>())
    CASE2(kRegDma2Control, return dmas[2].io.control.read<kIndex>())
    CASE2(kRegDma3Control, return dmas[3].io.control.read<kIndex>())

    default:
        UNREACHABLE;
        return 0;
    }
}

template<uint addr>
void DMAController::write(u8 byte)
{
    #define IMMEDIATE(dma) if (dma.io.control.reload) emit(dma, DMA::Timing::Immediate);

    #define MASK(value, index) (value >> (8 * index))

    switch (addr)
    {
    CASE4(kRegDma0Sad    , dmas[0].io.sad.write<kIndex>(byte & MASK(0x07FF'FFFF, kIndex)))
    CASE4(kRegDma1Sad    , dmas[1].io.sad.write<kIndex>(byte & MASK(0x0FFF'FFFF, kIndex)))
    CASE4(kRegDma2Sad    , dmas[2].io.sad.write<kIndex>(byte & MASK(0x0FFF'FFFF, kIndex)))
    CASE4(kRegDma3Sad    , dmas[3].io.sad.write<kIndex>(byte & MASK(0x0FFF'FFFF, kIndex)))
    CASE4(kRegDma0Dad    , dmas[0].io.dad.write<kIndex>(byte & MASK(0x07FF'FFFF, kIndex)))
    CASE4(kRegDma1Dad    , dmas[1].io.dad.write<kIndex>(byte & MASK(0x07FF'FFFF, kIndex)))
    CASE4(kRegDma2Dad    , dmas[2].io.dad.write<kIndex>(byte & MASK(0x07FF'FFFF, kIndex)))
    CASE4(kRegDma3Dad    , dmas[3].io.dad.write<kIndex>(byte & MASK(0x0FFF'FFFF, kIndex)))
    CASE2(kRegDma0Count  , dmas[0].io.count.write<kIndex>(byte & MASK(0x3FFF, kIndex)))
    CASE2(kRegDma1Count  , dmas[1].io.count.write<kIndex>(byte & MASK(0x3FFF, kIndex)))
    CASE2(kRegDma2Count  , dmas[2].io.count.write<kIndex>(byte & MASK(0x3FFF, kIndex)))
    CASE2(kRegDma3Count  , dmas[3].io.count.write<kIndex>(byte & MASK(0xFFFF, kIndex)))
    CASE2(kRegDma0Control, dmas[0].io.control.write<kIndex>(byte & MASK(0xF7E0, kIndex)); IMMEDIATE(dmas[0]))
    CASE2(kRegDma1Control, dmas[1].io.control.write<kIndex>(byte & MASK(0xF7E0, kIndex)); IMMEDIATE(dmas[1]))
    CASE2(kRegDma2Control, dmas[2].io.control.write<kIndex>(byte & MASK(0xF7E0, kIndex)); IMMEDIATE(dmas[2]))
    CASE2(kRegDma3Control, dmas[3].io.control.write<kIndex>(byte & MASK(0xFFE0, kIndex)); IMMEDIATE(dmas[3]))

    default:
        UNREACHABLE;
        break;
    }

    #undef MASK

    #undef IMMEDIATE
}
