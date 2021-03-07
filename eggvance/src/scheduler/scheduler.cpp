#include "scheduler.h"

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

        SCHEDULER_ASSERT(now - event.when);

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
    SCHEDULER_ASSERT(in);

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

void Scheduler::addIn(Event& event, u64 in)
{
    addAt(event, now + in);
}

void Scheduler::addAt(Event& event, u64 at)
{
    SCHEDULER_ASSERT(at);

    erase(event);
    event.when = at;

    if (events.empty())
    {
        events.push_back(event);
    }
    else
    {
        auto iter = events.rbegin();

        for (; iter != events.rend(); ++iter)
        {
            if (event < *iter)
                break;
        }
        events.insert(iter.base(), event);
    }
    next = events.back().when;
}

void Scheduler::erase(Event& event)
{
    if (event.when == 0)
        return;

    for (auto iter = events.begin(); iter != events.end(); ++iter)
    {
        if (event == *iter)
        {
            events.erase(iter);
            event.when = 0;
            return;
        }
    }
    SHELL_ASSERT(false);
}
