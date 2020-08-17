#pragma once

#include "timer/io.h"

class Core;

class Timer
{
public:
    Timer(Core& core, uint id);

    void init();
    void update();
    void run(int cycles);

    uint nextEvent() const;

    uint id{};
    Timer* prev{};
    Timer* next{};
    TimerIo io;

private:
    Core& core;

    uint counter{};
    uint initial{};
    uint overflow{};
};
