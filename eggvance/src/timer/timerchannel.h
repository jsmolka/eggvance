#pragma once

#include "io.h"

class TimerChannel
{
public:
    TimerChannel(uint id);

    void start();
    void update();
    void run(u64 cycles);
    void run();

    void schedule();
    u64 reschedule();

    const uint id;
    TimerCount count;
    TimerControl control;
    TimerChannel* pred = nullptr;
    TimerChannel* succ = nullptr;

//private:
    static void eventRun(void* data, u64 late);
    static void eventStart(void* data, u64 late);

    u64 event    = 0;
    u64 since    = 0;
    u64 counter  = 0;
    u64 initial  = 0;
    u64 overflow = 0;
};
