#include "scheduler.h"

void Scheduler::run(u64 cycles)
{
    now += cycles;

    while (now >= next)
    {
        const Event& event = queue.top();
        event.callback(event.data, now - event.at);

        queue.pop();
        next = queue.top().at;
    }
}

void Scheduler::add(u64 in, void* data, Event::Callback callback)
{
    queue.push({ now + in, data, callback });
    next = queue.top().at;
}
