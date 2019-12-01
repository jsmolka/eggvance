#include "videodevice.h"

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

std::shared_ptr<VideoDevice> video_device = std::make_shared<NoVideoDevice>();
