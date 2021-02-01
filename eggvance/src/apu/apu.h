#pragma once

#include "fifo.h"
#include "io.h"
#include "sequencer.h"

class Apu
{
public:
    void run(int cycles);
    void sample();
    void onTimerOverflow(uint timer, uint times);

    Fifo fifo[2];
    SoundBias bias;
    SoundControl control;
    Sequencer sequencer;

    int cycles = 0;
};

inline Apu apu;
