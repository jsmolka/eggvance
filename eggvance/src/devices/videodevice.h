#pragma once

#include <array>

#include "common/constants.h"
#include "common/integer.h"

class VideoDevice
{
public:
    virtual ~VideoDevice() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void present() = 0;
    virtual void fullscreen() = 0;

    u32 buffer[SCREEN_H * SCREEN_W];
};
