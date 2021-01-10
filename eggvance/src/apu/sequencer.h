#pragma once

#include "constants.h"
#include "noise.h"
#include "square1.h"
#include "square2.h"
#include "wave.h"

class Sequencer
{
public:
    static constexpr int kFrequency  = 512;
    static constexpr int kStepCycles = kCpuFrequency / kFrequency;

    void run(int cycles);

    Noise noise;
    Square1 square1;
    Square2 square2;
    Wave wave;

private:
    void sequence();

    int step = 0;
    int cycles = 0;
};
