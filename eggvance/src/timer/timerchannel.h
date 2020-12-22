#pragma once

#include "io.h"

class TimerChannel
{
public:
    TimerChannel(uint id);

    void start();
    void update();
    void run(int cycles);

    uint nextEvent() const;

    const uint id;
    TimerCount count;
    TimerControl control;
    TimerChannel* prev = nullptr;
    TimerChannel* next = nullptr;

private:
    uint delay    = 2;
    uint counter  = 0;
    uint initial  = 0;
    uint overflow = 0;
};
