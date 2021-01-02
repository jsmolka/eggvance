#include "apu.h"

#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"

#define s8_min (-127.0f)
#define s8_max (128.0f)
#define f32_min (-1.0f)
#define f32_max (1.0f)

inline float convert_sample(u8 sample)
{
    s8 ssample = sample;
    return ((((float)ssample - s8_min) * (f32_max - f32_min)) / (s8_max - s8_min)) + f32_min;
}


void callback(void* userdata, u8* stream, int length)
{
    Apu* apu = (Apu*)userdata;
    float* out = (float*)stream;
    for (int i = 0; i < length / sizeof(float); i++)
    {
        if (apu->audio.read_index < apu->audio.write_index)
            apu->last_sample = apu->audio.read();

        *out++ = apu->last_sample;
    }
}

Apu::Apu()
{
    dma_control.reset_fifo_a = [&]() { };
    dma_control.reset_fifo_b = [&]() { };

    if (config.bios_skip)
    {
        soundbias.value = 0x0200;
    }
}

void Apu::init()
{
    audio_ctx.open(this, callback);
}

void Apu::run(int cycles)
{
    this->cycles += cycles;

    while (this->cycles >= kSampleEveryCycles)
    {
        pushSample();

        this->cycles -= kSampleEveryCycles;
    }

}

void Apu::pushSample()
{
    u64 size = audio.write_index - audio.read_index;
    if (size < kAudioBufferSize)
    {
        audio.write(mix());
    }
}

float Apu::mix()
{
    float fifo0 = convert_sample(fifo[0].sample);
    float fifo1 = convert_sample(fifo[1].sample);

    return (fifo0 + fifo1) / 2;
}

void Apu::tickDmaSound(int channel)
{
    uint64_t wi = fifo[channel].write_index;
    uint64_t ri = fifo[channel].read_index;

    if (ri < wi)
    {
        fifo[channel].sample = fifo[channel].read();

        if (fifo[channel].size() <= 16)
        {
            dma.broadcast(channel == 0 ? Dma::Timing::FifoA : Dma::Timing::FifoB);
        }
    }
    else 
    {
        fifo[channel].sample = 0;
    }
}

void Apu::onTimerOverflow(uint id)
{
    if (dma_control.a.timer == id)
        tickDmaSound(0);

    if (dma_control.b.timer == id)
        tickDmaSound(1);
}
