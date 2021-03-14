#include "scheduler.h"

#include <limits>

Scheduler::Scheduler()
{
    next = std::numeric_limits<u64>::max();

    infinity.when = next;
    infinity = [this](u64 late)
    {
        SHELL_UNREACHABLE;
    };

    list.setHead(infinity);
}

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next)
    {
        Event& event = list.pop();
        event.when = 0;
        event.callback(now - next);

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
        next = list.head->when;
    }
}
