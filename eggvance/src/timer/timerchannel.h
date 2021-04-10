#pragma once

#include "io.h"
#include "scheduler/event.h"

class TimerChannel
{
public:
    TimerChannel(uint id);

    void start();
    void update();
    void schedule();
    void run();

    const uint id;
    TimerCount count;
    TimerControl control;
    TimerChannel* next = nullptr;

private:
    void run(u64 ticks);

    struct Events
    {
        Event run;
        Event start;
    } events;

    u64 since    = 0;
    u64 counter  = 0;
    u64 initial  = 0;
    u64 overflow = 1;
};
