#include "devices.h"

class NoAudioDevice : public AudioDevice
{
public:
    void init() override
    {

    }

    void deinit() override
    {

    }
};

class NoInputDevice : public InputDevice
{
public:
    void init() override
    {

    }

    void deinit() override
    {

    }
};

class NoVideoDevice : public VideoDevice
{
public:
    void init() override 
    {

    }

    void deinit() override
    {

    }

    void present() override
    {

    }

    void fullscreen() override
    {

    }
};

std::shared_ptr<AudioDevice> audio_device = std::make_shared<NoAudioDevice>();
std::shared_ptr<InputDevice> input_device = std::make_shared<NoInputDevice>();
std::shared_ptr<VideoDevice> video_device = std::make_shared<NoVideoDevice>();
