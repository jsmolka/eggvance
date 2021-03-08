#pragma once

#include "event.h"

#define SCHEDULER_ASSERT(value) SHELL_ASSERT(value < 0xFFFF'FFFF)

class Scheduler
{
public:
    Scheduler();

    void run(u64 cycles);

    void addIn(Event& event, u64 in);
    void addAt(Event& event, u64 at);
    void erase(Event& event);

    u64 now  = 0;
    u64 next = 0;

private:
    List<Event> list;
};

inline Scheduler scheduler;
