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

    void operator()(u64 now);
    bool operator<(const Event& other) const;
    bool operator>(const Event& other) const;
    bool scheduled() const;

    u64 when = 0;

private:
    std::function<void(u64)> callback;
};
