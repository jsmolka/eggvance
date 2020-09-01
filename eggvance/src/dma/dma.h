#pragma once

#include <array>

#include "dma/dmachannel.h"

class Dma
{
public:
    friend class Io;

    enum Timing
    {
        kTimingImmediate,
        kTimingVBlank,
        kTimingHBlank,
        kTimingSpecial
    };

    Dma();

    void run(int& cycles);
    void broadcast(Dma::Timing timing);

private:
    void emit(DmaChannel& channel, Dma::Timing timing);

    DmaChannel* active = nullptr;
    std::array<DmaChannel, 4> channels = { 0, 1, 2, 3 };
};

inline Dma dma;
