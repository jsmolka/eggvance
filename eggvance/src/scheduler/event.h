#pragma once

#include <functional>

#include "node.h"
#include "base/int.h"

class Event : public Node<Event>
{
public:
    template<typename Function>
    Event& operator=(Function&& func)
    {
        callback = std::forward<Function>(func);
        return *this;
    }

    bool operator<(const Event& other) const;
    bool operator>(const Event& other) const;
    bool isScheduled() const;

    u64 when = 0;
    std::function<void(u64)> callback;
};
