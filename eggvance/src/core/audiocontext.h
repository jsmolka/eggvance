#pragma once

class AudioContext
{
public:
    ~AudioContext();

    void init();

private:
    void deinit();
};

inline AudioContext audio_ctx;
