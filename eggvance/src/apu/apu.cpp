#include "apu.h"

#include <mutex>
#include <shell/utility.h>

#include "dma/dma.h"
#include "base/config.h"
#include "core/audiocontext.h"

std::mutex mutex;

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
    std::lock_guard<std::mutex> _(mutex);

    Apu* apu = (Apu*)userdata;
    float* out = (float*)stream;
    for (int i = 0; i < length / sizeof(float); i++)
    {
        if (apu->audio.read_index < apu->audio.write_index)
            apu->last_sample = apu->audio.read();
        *out++ = apu->last_sample * 0.1;
    }
}

Apu::Apu()
{
    dma_control.channels[0].clear_fifo = [&]() { fifo[0].clear(); };
    dma_control.channels[1].clear_fifo = [&]() { fifo[1].clear(); };

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
        sample();

        this->cycles -= kSampleEveryCycles;
    }

}

void Apu::sample()
{
    u64 size = audio.write_index - audio.read_index;
    if (size < kAudioBufferSize)
    {
        std::lock_guard<std::mutex> _(mutex);
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
    if (const auto sample = fifo[channel].read())
    {
        fifo[channel].sample = *sample;

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
    for (auto [fifo_id, channel] : shell::enumerate(dma_control.channels))
    {
        if (channel.timer == id)
            tickDmaSound(fifo_id);
    }
}
