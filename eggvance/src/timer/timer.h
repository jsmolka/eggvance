#pragma once

#include "timer/io.h"

class Timer
{
public:
    Timer(uint id);

    void init();
    void update();
    void run(int cycles);

    uint nextOverflow() const;

    uint id{};
    Timer* next{};
    TimerIo io;

private:
    uint counter{};
    uint initial{};
    uint overflow{};
};
