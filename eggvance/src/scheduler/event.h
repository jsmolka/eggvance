#pragma once

#include "base/int.h"
#include "base/macros.h"

class Event
{
public:
    using Callback = void(*)(void*, u64);

    bool operator==(const Event& event) const { return at == event.at && callback == event.callback; }
    bool operator> (const Event& event) const { return at >  event.at; }

    u64 at;
    void* data;
    Callback callback;
};
