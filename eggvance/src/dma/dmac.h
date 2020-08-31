#pragma once

#include "base/constants.h"
#include "base/macros.h"
#include "dma/dma.h"

class DmaController
{
public:
    void run(int& cycles);
    void broadcast(Dma::Timing timing);

    template<uint Addr> u8 read() const;
    template<uint Addr> void write(u8 byte);

private:
    void emit(Dma& dma, Dma::Timing timing);

    Dma* active{};
    Dma dmas[4] = { 0, 1, 2, 3 };
};

inline DmaController dmac;

template<uint Addr>
u8 DmaController::read() const
{    
    INDEXED_IF2(Addr, kRegDma0Control, return dmas[0].io.control.read<kIndex>());
    INDEXED_IF2(Addr, kRegDma1Control, return dmas[1].io.control.read<kIndex>());
    INDEXED_IF2(Addr, kRegDma2Control, return dmas[2].io.control.read<kIndex>());
    INDEXED_IF2(Addr, kRegDma3Control, return dmas[3].io.control.read<kIndex>());

    EGGCPT_UNREACHABLE;
    return 0;
}

template<uint Addr>
void DmaController::write(u8 byte)
{
    #define EPILOGUE(dma)                       \
        if (dma.io.control.reload)              \
            emit(dma, Dma::kTimingImmediate)

    INDEXED_IF4(Addr, kRegDma0Sad    , dmas[0].io.sad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_IF4(Addr, kRegDma0Dad    , dmas[0].io.dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_IF2(Addr, kRegDma0Count  , dmas[0].io.count.write<kIndex, 0x3FFF>(byte));
    INDEXED_IF2(Addr, kRegDma0Control, dmas[0].io.control.write<kIndex, 0xF7E0>(byte); EPILOGUE(dmas[0]));
    INDEXED_IF4(Addr, kRegDma1Sad    , dmas[1].io.sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_IF4(Addr, kRegDma1Dad    , dmas[1].io.dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_IF2(Addr, kRegDma1Count  , dmas[1].io.count.write<kIndex, 0x3FFF>(byte));
    INDEXED_IF2(Addr, kRegDma1Control, dmas[1].io.control.write<kIndex, 0xF7E0>(byte); EPILOGUE(dmas[1]));
    INDEXED_IF4(Addr, kRegDma2Sad    , dmas[2].io.sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_IF4(Addr, kRegDma2Dad    , dmas[2].io.dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_IF2(Addr, kRegDma2Count  , dmas[2].io.count.write<kIndex, 0x3FFF>(byte));
    INDEXED_IF2(Addr, kRegDma2Control, dmas[2].io.control.write<kIndex, 0xF7E0>(byte); EPILOGUE(dmas[2]));
    INDEXED_IF4(Addr, kRegDma3Sad    , dmas[3].io.sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_IF4(Addr, kRegDma3Dad    , dmas[3].io.dad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_IF2(Addr, kRegDma3Count  , dmas[3].io.count.write<kIndex, 0xFFFF>(byte));
    INDEXED_IF2(Addr, kRegDma3Control, dmas[3].io.control.write<kIndex, 0xFFE0>(byte); EPILOGUE(dmas[3]));

    #undef EPILOGUE
}
