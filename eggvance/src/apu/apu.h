#pragma once

#include "io.h"
#include "fifo.h"

class Apu
{
public:
    Apu();

    SHELL_INLINE void run(int cycles)
    {
        constexpr auto kCpuFrequency = 16 * 1024 * 1024;
        constexpr auto kSampleRate   = 32 * 1024;
        constexpr auto kSampleCycles = kCpuFrequency / kSampleRate;

        this->cycles += cycles;

        while (this->cycles >= kSampleCycles)
        {
            sample();

            this->cycles -= kSampleCycles;
        }
    }

    void sample();
    void onTimerOverflow(uint id, uint times);

    Fifo fifo[2];
    SoundBias sound_bias;
    SoundControl sound_enable;
    DirectSoundControl direct_sound;

    Register<u16, 0x007F> soundcnt1l;
    Register<u16, 0xFFC0> soundcnt1h;
    Register<u16, 0x4000> soundcnt1x;
    Register<u16, 0xFFC0> soundcnt2l;
    Register<u16, 0x4000> soundcnt2h;
    Register<u16, 0x00E0> soundcnt3l;
    Register<u16, 0xE000> soundcnt3h;
    Register<u16, 0x4000> soundcnt3x;
    Register<u16, 0xFF00> soundcnt4l;
    Register<u16, 0x40FF> soundcnt4h;
    Register<u16, 0xFF77> soundcntl;
    Register<u16> waveram[8];

    int cycles = 0;
};

inline Apu apu;
