#pragma once

#include "timer/io.h"

class TimerChannel
{
public:
    TimerChannel(uint id)
        : id(id) {}

    void start();
    void update();
    void run(int cycles);

    uint nextEvent() const;

    uint id;
    TimerCount count;
    TimerControl control;
    TimerChannel* prev = nullptr;
    TimerChannel* next = nullptr;

private:
    uint counter  = 0;
    uint initial  = 0;
    uint overflow = 0;
};
