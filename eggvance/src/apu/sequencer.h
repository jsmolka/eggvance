#pragma once

#include "noise.h"
#include "square1.h"
#include "square2.h"
#include "wave.h"

class Sequencer
{
public:
    Sequencer();

    void tick();

    Noise noise;
    Square1 square1;
    Square2 square2;
    Wave wave;

private:
    template<uint Step>
    static void sequence(void* data, u64);

    void tickSweep();
    void tickLength();
    void tickEnvelope();
};
