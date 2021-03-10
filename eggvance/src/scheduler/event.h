#pragma once

#include "node.h"
#include "base/int.h"

class Event : public Node<Event>
{
public:
    using Callback = void(*)(void*, u64);

    bool operator<(const Event& other) { return when < other.when; }
    bool operator>(const Event& other) { return when > other.when; }

    u64 when = 0;
    void* data = nullptr;
    Callback callback = nullptr;
};
