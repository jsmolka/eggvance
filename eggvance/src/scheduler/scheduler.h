#pragma once

#include <vector>

#include "event.h"

class Scheduler
{
public:
    Scheduler();

    u64 now = 0;
    u64 next = 0;

    void run(u64 cycles);
    void add(u64 in, void* data, Event::Callback callback);

private:
    std::vector<Event> events;
};

inline Scheduler scheduler;
