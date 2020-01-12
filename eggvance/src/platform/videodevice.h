#pragma once

#include <memory>
#include <string>

#include "device.h"
#include "common/constants.h"
#include "common/integer.h"

class VideoDevice : public Device
{
public:
    virtual void present() = 0;
    virtual void fullscreen() = 0;
    virtual void title(const std::string& title) = 0;

    u32 buffer[SCREEN_H * SCREEN_W];
};

extern std::shared_ptr<VideoDevice> video_device;
