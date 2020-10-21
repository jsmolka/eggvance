#pragma once

#include <array>

#include "dmachannel.h"

class Dma
{
public:
    friend class Io;

    Dma();

    void run(int& cycles);
    void broadcast(DmaControl::Timing timing);

private:
    void emit(DmaChannel& channel, DmaControl::Timing timing);

    DmaChannel* active = nullptr;
    std::array<DmaChannel, 4> channels = { 0, 1, 2, 3 };
};

inline Dma dma;
