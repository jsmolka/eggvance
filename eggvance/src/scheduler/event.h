#pragma once

#include "base/int.h"
#include "base/macros.h"

class Event
{
public:
    using Callback = void(*)(void*, u64);

    bool operator==(const Event& event) const { return when == event.when && callback == event.callback; }
    bool operator> (const Event& event) const { return when >  event.when; }
    bool operator< (const Event& event) const { return when <  event.when; }

    u64 when;
    void* data;
    Callback callback;
};
