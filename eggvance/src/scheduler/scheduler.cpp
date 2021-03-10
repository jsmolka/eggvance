#include "scheduler.h"

#include <limits>

Scheduler::Scheduler()
{
    events.tombstone.data = this;
    events.tombstone.callback = &Events::die;
    events.tombstone.when = std::numeric_limits<u64>::max();

    list.setTombstone(events.tombstone);

    next = events.tombstone.when;
}

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next)
    {
        Event& event = list.pop();

        u64 late = now - event.when;
        SCHEDULER_ASSERT(late);

        event.when = 0;
        event.callback(event.data, late);

        next = list.head->when;
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

void Scheduler::Events::die(void* data, u64 late)
{
    SHELL_UNREACHABLE;
}
