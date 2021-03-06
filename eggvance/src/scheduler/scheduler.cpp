#include "scheduler.h"

#include <limits>

Scheduler::Scheduler()
{
    events.reserve(64);

    next = std::numeric_limits<u64>::max();
}

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next)
    {
        auto& event = events.back();
        event.callback(event.data, now - event.when);

        events.pop_back();

        if (events.empty())
            next = std::numeric_limits<u64>::max();
        else
            next = events.back().when;
    }
}

u64 Scheduler::add(u64 in, void* data, Event::Callback callback)
{
    SHELL_ASSERT(in < std::numeric_limits<u32>::max());

    Event event = { now + in, data, callback };

    if (events.empty())
    {
        events.push_back(event);
    }
    else
    {
        // https://dirtyhandscoding.wordpress.com/2017/08/25/performance-comparison-linear-search-vs-binary-search/

        auto iter = events.rbegin();

        for (; iter != events.rend(); ++iter)
        {
            if (event < *iter)
                break;
        }
        events.insert(iter.base(), event);
    }
    next = events.back().when;

    return event.when;
}

void Scheduler::remove(const Event& event)
{
    for (auto iter = events.begin(); iter != events.end(); ++iter)
    {
        if (event == *iter)
        {
            events.erase(iter);
            return;
        }
    }
}
