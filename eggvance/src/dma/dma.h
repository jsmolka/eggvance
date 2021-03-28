#pragma once

#include "dmachannel.h"
#include "base/array.h"

class Dma
{
public:
    enum class Event
    {
        Immediate,
        HBlank,
        VBlank,
        Hdma,
        FifoA,
        FifoB
    };

    void run();
    void emit(DmaChannel& channel, Event event);
    void broadcast(Event event);

    array<DmaChannel, 4> channels = { 0, 1, 2, 3 };

private:
    DmaChannel* active = nullptr;
};

inline Dma dma;
