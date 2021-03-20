#pragma once

#include "dmachannel.h"

class Dma
{
public:
    enum class Event { Immediate, HBlank, VBlank, FifoA, FifoB, Hdma };

    Dma();

    void run();
    void broadcast(Event event);

    DmaChannel channels[4] = { 0, 1, 2, 3 };

private:
    void emit(DmaChannel& channel, Event event);

    DmaChannel* active = nullptr;
};

inline Dma dma;
