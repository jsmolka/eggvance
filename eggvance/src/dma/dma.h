#pragma once

#include "dmachannel.h"

class Dma
{
public:
    enum class Event { Immediate, HBlank, VBlank, Hdma, FifoA, FifoB };

    void run();
    void emit(DmaChannel& channel, Event event);
    void broadcast(Event event);

    DmaChannel channels[4] = { 0, 1, 2, 3 };

private:
    DmaChannel* active = nullptr;
};

inline Dma dma;
