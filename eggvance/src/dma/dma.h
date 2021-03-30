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
        FifoA,
        FifoB,
        Hdma
    };

    void run();
    void emit(DmaChannel& channel, Event event);
    void broadcast(Event event);

    array<DmaChannel, 4> channels = { 0, 1, 2, 3 };

private:
    static bool triggers(const DmaChannel& channel, Event event);

    DmaChannel* active = nullptr;
};

inline Dma dma;
