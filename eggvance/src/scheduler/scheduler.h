#pragma once

#include "circularlist.h"
#include "event.h"

class Scheduler
{
public:
    Scheduler();

    void run(u64 cycles);
    void queueIn(Event& event, u64 in);
    void dequeue(Event& event);

    u64 now  = 0;
    u64 next = 0;

private:
    struct Events
    {
        static void doTombstone(void* data, u64 late);

        Event tombstone;
    } events;

    CircularList<Event> list;
};

inline Scheduler scheduler;
