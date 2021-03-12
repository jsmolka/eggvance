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
    TimerChannel* prev = nullptr;
    TimerChannel* next = nullptr;

private:
    struct Events
    {
        Event run;
        Event start;
    } events;

    void run(u64 ticks);

    u64 since    = 0;
    u64 counter  = 0;
    u64 initial  = 0;
    u64 overflow = 1;
};
