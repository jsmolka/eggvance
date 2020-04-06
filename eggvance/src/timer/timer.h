#pragma once

#include "io/timerio.h"

class Timer
{
public:
    Timer(uint id);

    void start();
    void update();
    void run(uint cycles);

    uint nextOverflow() const;

    uint id;
    Timer* next = nullptr;
    TimerIO io;

private:
    uint counter  = 0;
    uint initial  = 0;
    uint overflow = 0;
};
