#include "scheduler.h"

#include <limits>

Scheduler::Scheduler()
{
    next = std::numeric_limits<u64>::max();
}

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next && list.head)
    {
        Event& event = list.pop();

        u64 late = now - event.when;
        SCHEDULER_ASSERT(late);

        event.when = 0;
        event.callback(event.data, late);

        if (list.head)
            next = list.head->when;
        else
            next = std::numeric_limits<u64>::max();
    }
}

void Scheduler::addIn(Event& event, u64 in)
{
    addAt(event, now + in);
}

void Scheduler::addAt(Event& event, u64 at)
{
    SCHEDULER_ASSERT(at);
    SHELL_ASSERT(event.when == 0);

    event.when = at;

    list.insert(event);

    next = list.head->when;
}

void Scheduler::erase(Event& event)
{
    if (event.when)
    {
        list.erase(event);
        event.when = 0;
    }
}
