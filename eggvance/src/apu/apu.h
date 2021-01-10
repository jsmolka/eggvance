#pragma once

#include "fifo.h"
#include "io.h"
#include "sequencer.h"

class Apu
{
public:
    Apu();

    void run(int cycles);
    void sample();
    void onTimerOverflow(uint id, uint times);

    Fifo fifo[2];
    SoundBias sound_bias;
    SoundControl sound_enable;
    DirectSoundControl direct_sound;
    Sequencer sequencer;

    Register<u16, 0xFF77> soundcntl;
    Register<u16> waveram[8];

    int cycles = 0;
};

inline Apu apu;
