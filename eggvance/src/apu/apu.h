#pragma once

#include "fifo.h"
#include "io.h"
#include "noise.h"
#include "square1.h"
#include "square2.h"
#include "wave.h"

class Apu
{
public:
    Apu();

    void run(int cycles);
    void onTimerOverflow(uint timer, uint times);

    Fifo fifo[2];
    SoundBias bias;
    SoundControl control;
    Noise noise;
    Square1 square1;
    Square2 square2;
    Wave wave;

private:
    template<uint Step>
    static void sequence(void*, u64 late);
    static void sample(void*, u64 late);
};

inline Apu apu;
