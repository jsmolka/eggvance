#include "scheduler.h"

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next && !queue.empty())
    {
        const Event& event = queue.top();
        event.callback(event.data, now - event.at);

        queue.pop();

        if (queue.empty())
            next = std::numeric_limits<u64>::max();
        else
            next = queue.top().at;
    }
}

void Scheduler::add(u64 in, void* data, Event::Callback callback)
{
    SHELL_ASSERT(in < std::numeric_limits<u64>::max() / 2);

    queue.push({ now + in, data, callback });
    next = queue.top().at;
}
