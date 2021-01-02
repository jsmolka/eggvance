#pragma once

#include <list>

#include "io.h"

constexpr auto kFifoSize = 32;
constexpr auto kAudioSampleRate = 48000;
constexpr auto kAudioBufferSize = 4096;
constexpr auto kCpuFrequency = 16 * 1024 * 1024;
constexpr auto kSampleEveryCycles = kCpuFrequency / kAudioSampleRate;

template<typename T, std::size_t N>
struct RingBuffer
{
    T read()
    {
        return buf[(read_index++) % N];
    }

    void write(const T& value)
    {
        buf[(write_index++) % N] = value;
    }

    int size()
    {
        return write_index - read_index;
    }

    T buf[N];
    u64 read_index = 0;
    u64 write_index = 1;
};

struct Fifo : RingBuffer<u8, kFifoSize>
{
    void writeWord(u32 value)
    {
        if (size() <= 28)
        {
            write(bit::seq< 0, 8>(value));
            write(bit::seq< 8, 8>(value));
            write(bit::seq<16, 8>(value));
            write(bit::seq<24, 8>(value));
        }
    }

    u8 sample = 0;
};

struct AudioBuffer : RingBuffer<float, kAudioBufferSize>
{

};

class Apu
{
public:
    Apu();

    void init();
    void run(int cycles);
    void pushSample();
    float mix();
    void tickDmaSound(int channel);
    void onTimerOverflow(uint id);

    Fifo fifo[2];
    AudioBuffer audio;
    float last_sample;

    DmaSoundControl dma_control;    

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
    Register<u16, 0x0080> soundcntx;
    Register<u16> soundbias;
    Register<u16> waveram[8];

    int cycles = 0;
};

inline Apu apu;
