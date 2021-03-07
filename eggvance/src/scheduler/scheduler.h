#pragma once

#include <vector>

#include "event.h"

#define SCHEDULER_ASSERT(value) SHELL_ASSERT(value < 0xFFFF'FFFF)

class Scheduler
{
public:
    Scheduler();

    void run(u64 cycles);
    
    u64 add(u64 in, void* data, Event::Callback callback);

    void addIn(Event& event, u64 in);
    void addAt(Event& event, u64 at);
    void erase(Event& event);

    u64 now  = 0;
    u64 next = 0;

private:
    std::vector<Event> events;
};

inline Scheduler scheduler;
