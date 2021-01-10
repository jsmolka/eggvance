#pragma once

#include "wave.h"
#include "apu/constants.h"

class Sequencer
{
public:
    static constexpr int kFrequency  = 512;
    static constexpr int kStepCycles = kCpuFrequency / kFrequency;

    void run(int cycles);

    Wave wave;

private:
    void sequence();

    int step = 0;
    int cycles = 0;
};
