#pragma once

#include "audiodevice.h"
#include "inputdevice.h"
#include "videodevice.h"

namespace common
{
    void init(
        int argc, char* argv[],
        const std::shared_ptr<AudioDevice>& audio_device,
        const std::shared_ptr<InputDevice>& input_device,
        const std::shared_ptr<VideoDevice>& video_device
    );

    void reset();
    void frame();
}
