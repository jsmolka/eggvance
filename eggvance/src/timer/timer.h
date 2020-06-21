#pragma once

#include "timer/io.h"

class Timer
{
public:
    Timer(uint id);

    void init();
    void update();
    void run(int cycles);

    uint nextEvent() const;

    uint id{};
    Timer* prev{};
    Timer* next{};
    TimerIo io;

private:
    uint counter{};
    uint initial{};
    uint overflow{};
};
