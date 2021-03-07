#pragma once

#include "io.h"
#include "scheduler/event.h"

class TimerChannel
{
public:
    TimerChannel(uint id);

    void start();
    void update();
    void run(u64 ticks);
    void run();

    const uint id;
    TimerCount count;
    TimerControl control;
    TimerChannel* pred = nullptr;
    TimerChannel* succ = nullptr;

private:
    struct Events
    {
        static void doRun(void* data, u64 late);
        static void doStart(void* data, u64 late);

        Event run;
        Event start;
    } events;

    void schedule();

    u64 since    = 0;
    u64 counter  = 0;
    u64 initial  = 0;
    u64 overflow = 0;
};
