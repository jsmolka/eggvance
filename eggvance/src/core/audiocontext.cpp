#include "audiocontext.h"

#include "base/panic.h"

AudioContext::~AudioContext()
{
    deinit();
}

void AudioContext::init()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        panic("Cannot init audio context {}", SDL_GetError());

    SDL_AudioSpec want = {};
    SDL_AudioSpec have = {};

    #if RESAMPLE_AUDIO
    want.freq     = 44100;
    want.samples  = kSamples;
    want.format   = AUDIO_F32SYS;
    want.channels = 2;
    want.userdata = this;
    want.callback = callback;
    #else
    want.freq     = kSampleRate;
    want.samples  = kSamples;
    want.format   = AUDIO_S16SYS;
    want.channels = 2;
    want.userdata = this;
    want.callback = callback;
    #endif

    device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

    if (!device)
        panic("Cannot init audio device {}", SDL_GetError());

    #if RESAMPLE_AUDIO
    stream = SDL_NewAudioStream(AUDIO_S16, 2, kSampleRate, have.format, have.channels, have.freq);

    if (!stream)
        panic("Cannot init audio stream {}", SDL_GetError());
    #endif

    resume();
}

void AudioContext::pause()
{
    SDL_PauseAudioDevice(device, true);
}

void AudioContext::resume()
{
    SDL_PauseAudioDevice(device, false);
}

void AudioContext::write(const Sample& sample)
{
    std::lock_guard lock(mutex);
    #if RESAMPLE_AUDIO
    if (SDL_AudioStreamAvailable(stream) < 0.25 * kSampleRate * sizeof(float))
        SDL_AudioStreamPut(stream, reinterpret_cast<const void*>(&sample), sizeof(Sample));
    #else
    buffer.write(sample);
    #endif
}

void AudioContext::clear()
{
    #if RESAMPLE_AUDIO
    SDL_AudioStreamClear(stream);
    #else
    buffer.clear();
    #endif
}

void AudioContext::callback(void* userdata, u8* stream, int length)
{
    AudioContext& self = *reinterpret_cast<AudioContext*>(userdata);
    #if RESAMPLE_AUDIO
    int gotten = 0;
    {
        std::lock_guard lock(self.mutex);
        if (SDL_AudioStreamAvailable(self.stream))
            gotten = SDL_AudioStreamGet(self.stream, stream, length);
    }

    if (gotten < length)
    {
        auto f_sample = 0.0f;
        auto f_stream = reinterpret_cast<float*>(stream);
        auto f_gotten = gotten / sizeof(float);
        auto f_length = length / sizeof(float);

        if (f_gotten)
            f_sample = f_stream[f_gotten - 1];

        std::fill(f_stream + f_gotten, f_stream + f_length, f_sample);
    }
    #else
    auto s_sample = Sample();
    auto s_stream = reinterpret_cast<Sample*>(stream);
    auto s_gotten = self.buffer.size();
    auto s_length = length / sizeof(Sample);

    if (s_gotten)
        s_sample = self.buffer.back();

    {
        std::lock_guard lock(self.mutex);
        std::copy(self.buffer.begin(), self.buffer.end(), s_stream);
        self.buffer.clear();
    }
    std::fill(s_stream + s_gotten, s_stream + s_length, s_sample);
    #endif
}

void AudioContext::deinit()
{

    if (SDL_WasInit(SDL_INIT_AUDIO))
    {
        #if RESAMPLE_AUDIO
        if (stream)
            SDL_FreeAudioStream(stream);
        #endif

        SDL_CloseAudioDevice(device);

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}
