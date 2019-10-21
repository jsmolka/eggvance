#pragma once

#include "mmu/registers/tmcnt.h"

class Timer
{
public:
    void reset();

    void run(int cycles);

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int id;
    Timer* next;

private:
    void runInternal(int cycles);
    void updateData();
    void calculate();

    constexpr static int prescalers[4] = { 1, 64, 256, 1024 };

    int counter;
    int overflow;

    TmCnt control;
};
