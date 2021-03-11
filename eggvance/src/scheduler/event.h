#pragma once

#include "node.h"
#include "base/int.h"

class Event : public Node<Event>
{
public:
    using Callback = void(*)(void*, u64);

    void operator()(u64 now);
    bool operator<(const Event& other) const;
    bool operator>(const Event& other) const;
    bool scheduled() const;

    u64 when = 0;
    void* data = nullptr;
    Callback callback = nullptr;
};
