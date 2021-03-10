#pragma once

#include "node.h"
#include "base/int.h"

class Event : public Node<Event>
{
public:
    using Callback = void(*)(void*, u64);

    void operator()(u64 now)
    {
        u64 late = now - when;

        when = 0;
        callback(data, late);
    }
    
    bool operator<(const Event& other) const
    {
        return when < other.when;
    }

    bool operator>(const Event& other) const
    {
        return when > other.when;
    }

    u64 when = 0;
    void* data = nullptr;
    Callback callback = nullptr;
};
