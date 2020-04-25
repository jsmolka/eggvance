#pragma once

#include <memory>
#include <string>

#include "base/constants.h"
#include "base/integer.h"

class VideoDevice
{
public:
    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void present() = 0;
    virtual void fullscreen() = 0;
    virtual void setWindowTitle(const std::string& title) = 0;

    u32 buffer[kScreenH * kScreenW];
};

extern std::shared_ptr<VideoDevice> video_device;
