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
    void onOverflow(uint timer, uint ticks);

    Square1 square1;
    Square2 square2;
    Wave wave;
    Noise noise;
    shell::array<Fifo, 2> fifos;
    SoundControl control;
    SoundBias bias;

private:
    template<uint kStep>
    void sequence(u64 late);
    void sample(u64 late);

    struct Events
    {
        Event sequence;
        Event sample;
    } events;
};

inline Apu apu;
