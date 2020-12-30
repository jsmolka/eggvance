#pragma once

#include "dmachannel.h"

class Dma
{
public:
    enum class Timing { Immediate, VBlank, HBlank, Audio, Video };

    Dma();

    void run();
    void broadcast(Timing timing);

    DmaChannel channels[4] = { 0, 1, 2, 3 };

private:
    void emit(DmaChannel& channel, Timing timing);

    DmaChannel* active = nullptr;
};

inline Dma dma;
