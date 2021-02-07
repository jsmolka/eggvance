#pragma once

#include "noise.h"
#include "square1.h"
#include "square2.h"
#include "wave.h"

class Sequencer
{
public:
    void tick();

    Noise noise;
    Square1 square1;
    Square2 square2;
    Wave wave;

private:

    uint step   = 0;
    uint cycles = 0;
};
