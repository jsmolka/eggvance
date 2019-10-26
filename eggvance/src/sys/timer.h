#pragma once

#include "regs/timerdata.h"
#include "regs/timercontrol.h"

class Timer
{
public:
    void reset();

    void run(int cycles);

    u8 read(int index);
    void write(int index, u8 byte);

    int id;
    Timer* next;

    TimerData data;
    TimerControl control;


private:
    void runInternal(int cycles);
    void updateData();
    void calculate();

    constexpr static int prescalers[4] = { 1, 64, 256, 1024 };

    int counter;
    int overflow;
};
