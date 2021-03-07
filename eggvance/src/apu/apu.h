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
        template<uint Step>
        static void doSequence(void* data, u64 late);
        static void doSample(void* data, u64 late);

        Event sequence;
        Event sample;
    } events;
};

inline Apu apu;
