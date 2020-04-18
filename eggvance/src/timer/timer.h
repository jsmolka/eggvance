#pragma once

#include "io/timerio.h"

class Timer
{
public:
    Timer(int id);

    void init();
    void update();
    void run(int cycles);

    int nextOverflow() const;

    int id;
    Timer* next = nullptr;
    TimerIO io;

private:
    int counter  = 0;
    int initial  = 0;
    int overflow = 0;
};
