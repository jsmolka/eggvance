#include "apu.h"

#include "base/config.h"
#include "core/audiocontext.h"

void callback(void* userdata, u8* stream8, int len8)
{
    s16* stream = reinterpret_cast<s16*>(stream8);
    int len = len8 / 2;

    #if SHELL_DEBUG
    std::time_t t = std::time(nullptr);
    fmt::print("Sample {:%S} {}\n", fmt::localtime(t), len);
    #endif


    for (int i = 0; i < len; ++i)
        stream[i] = (rand() % 65533) - 32500;

    for (int i = 0; i < len; ++i)
        stream[i] = static_cast<s16>(stream[i] * 0.01);
}

Apu::Apu()
{
    dma_control.reset_fifo_a = []() {};
    dma_control.reset_fifo_b = []() {};

    if (config.bios_skip)
    {
        soundbias.value = 0x0200;
    }
}

void Apu::init()
{
    audio_ctx.open(this, callback);
}
