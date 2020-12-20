#pragma once

#include "dmachannel.h"

class Dma
{
public:
    Dma();

    void run();
    void broadcast(DmaControl::Timing timing);

    DmaChannel channels[4] = { 0, 1, 2, 3 };

private:
    void emit(DmaChannel& channel, DmaControl::Timing timing);

    DmaChannel* active = nullptr;
};

inline Dma dma;
