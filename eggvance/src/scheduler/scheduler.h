#pragma once

#include "event.h"
#include "queue.h"

class Scheduler
{
public:
    u64 now = 0;
    u64 next = 0;

    void run(u64 cycles);
    void add(u64 in, void* data, Event::Callback callback);

private:
    Queue<Event> queue;
};

inline Scheduler scheduler;
