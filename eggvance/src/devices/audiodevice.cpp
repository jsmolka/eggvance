#include "audiodevice.h"

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

std::shared_ptr<AudioDevice> audio_device = std::make_shared<NoAudioDevice>();
