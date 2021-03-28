#pragma once

#include "circularlist.h"
#include "event.h"

class Scheduler
{
public:
    Scheduler();

    void run(u64 cycles);
    void insert(Event& event, u64 in);
    void remove(Event& event);

    u64 now  = 0;
    u64 next = 0;

private:
    Event infinity;
    CircularList<Event> list;
};

inline Scheduler scheduler;
