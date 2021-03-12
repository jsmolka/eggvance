#pragma once

#include "fifo.h"
#include "io.h"
#include "noise.h"
#include "square1.h"
#include "square2.h"
#include "wave.h"
#include "scheduler/event.h"

class Apu
{
public:
    Apu();

    void init();
    void onOverflow(uint timer, uint times);

    SoundControl control;
    SoundBias bias;
    Fifo fifo[2];
    Noise noise;
    Square1 square1;
    Square2 square2;
    Wave wave;

private:
    struct Events
    {
        Event sequence;
        Event sample;
    } events;

    template<uint Step>
    void sequence(u64 late);
    void sample(u64 late);
};

inline Apu apu;
