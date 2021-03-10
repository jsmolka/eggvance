#include "scheduler.h"

#include <limits>

Scheduler::Scheduler()
{
    events.tombstone.data = this;
    events.tombstone.callback = &Events::doTombstone;
    events.tombstone.when = std::numeric_limits<u64>::max();

    list.setTombstone(events.tombstone);

    next = events.tombstone.when;
}

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next)
    {
        list.pop()(now);

        next = list.head->when;
    }
}

void Scheduler::queueIn(Event& event, u64 in)
{
    SHELL_ASSERT(event.when == 0);
    SHELL_ASSERT(static_cast<s64>(in) > 0);

    event.when = now + in;

    list.insert(event);

    next = list.head->when;
}

void Scheduler::dequeue(Event& event)
{
    if (event.when)
    {
        event.when = 0;
        list.erase(event);
    }
}

void Scheduler::Events::doTombstone(void* data, u64 late)
{
    SHELL_UNREACHABLE;
}
