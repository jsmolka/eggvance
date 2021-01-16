#pragma once

#include "constants.h"
#include "noise.h"
#include "square1.h"
#include "square2.h"
#include "wave.h"

class Sequencer
{
public:
    void run(int cycles);

    Noise noise;
    Square1 square1;
    Square2 square2;
    Wave wave;

private:
    void tick();

    uint step   = 0;
    uint cycles = 0;
};
